#include "XTabBar.h"

#include <QFontMetrics>
#include <QIcon>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOptionTab>
#include <QStyleOptionTabBarBase>
#include <QStylePainter>

namespace
{
    bool isHorizontalShape(QTabBar::Shape shape)
    {
        switch (shape)
        {
        case QTabBar::RoundedNorth:
        case QTabBar::RoundedSouth:
        case QTabBar::TriangularNorth:
        case QTabBar::TriangularSouth:
            return true;
        default:
            return false;
        }
    }
}

// ---------------------------------------------------------------------------
XTabBar::XTabBar(QWidget* parent) : QTabBar(parent)
{
}

Qt::ToolButtonStyle XTabBar::toolButtonStyle() const
{
    return m_style;
}

void XTabBar::setToolButtonStyle(Qt::ToolButtonStyle style)
{
    if (m_style == style)
        return;
    m_style = style;
    relayoutTabs();
    emit toolButtonStyleChanged(style);
}

void XTabBar::relayoutTabs()
{
    // QTabBar only recomputes tab rects when its private "layoutDirty" flag is
    // set. setIconSize() does that unconditionally, even with the same size.
    setIconSize(iconSize());
    updateGeometry();
    update();
}

int XTabBar::iconTextSpacing() const
{
    return m_spacing;
}

void XTabBar::setIconTextSpacing(int spacing)
{
    spacing = qMax(0, spacing);
    if (m_spacing == spacing)
        return;
    m_spacing = spacing;
    relayoutTabs();
}

QMargins XTabBar::tabPadding() const
{
    return m_padding;
}

void XTabBar::setTabPadding(const QMargins& padding)
{
    const QMargins clean(qMax(0, padding.left()), qMax(0, padding.top()),
                         qMax(0, padding.right()), qMax(0, padding.bottom()));
    if (m_padding == clean)
        return;
    m_padding = clean;
    relayoutTabs();
}

void XTabBar::setTabPadding(int left, int top, int right, int bottom)
{
    setTabPadding(QMargins(left, top, right, bottom));
}

int XTabBar::tabPaddingLeft() const { return m_padding.left(); }
int XTabBar::tabPaddingTop() const { return m_padding.top(); }
int XTabBar::tabPaddingRight() const { return m_padding.right(); }
int XTabBar::tabPaddingBottom() const { return m_padding.bottom(); }

void XTabBar::setTabPaddingLeft(int left)     { QMargins m = m_padding; m.setLeft(left);     setTabPadding(m); }
void XTabBar::setTabPaddingTop(int top)       { QMargins m = m_padding; m.setTop(top);       setTabPadding(m); }
void XTabBar::setTabPaddingRight(int right)   { QMargins m = m_padding; m.setRight(right);   setTabPadding(m); }
void XTabBar::setTabPaddingBottom(int bottom) { QMargins m = m_padding; m.setBottom(bottom); setTabPadding(m); }

bool XTabBar::usesCustomLayout() const
{
    return (m_style != Qt::ToolButtonFollowStyle || !m_padding.isNull()) && isHorizontalShape(shape());
}

Qt::ToolButtonStyle XTabBar::effectiveStyle() const
{
    return m_style == Qt::ToolButtonFollowStyle ? Qt::ToolButtonTextBesideIcon : m_style;
}

// ---------------------------------------------------------------------------
// Size hints
//
// QTabBar::tabSizeHint() assumes "icon beside text":
//     width  = textW + iconW + spacing + paddings (+ close/side buttons)
//     height = max(textH, iconH) + paddings
// The other layouts are derived from it so style paddings stay correct.
// ---------------------------------------------------------------------------
QSize XTabBar::tabSizeHint(int index) const
{
    QSize size = QTabBar::tabSizeHint(index);
    if (!usesCustomLayout())
        return size;

    // our extra padding applies to every custom layout
    size.rwidth() += m_padding.left() + m_padding.right();
    size.rheight() += m_padding.top() + m_padding.bottom();

    const Qt::ToolButtonStyle style = effectiveStyle();
    const bool hasIcon = !tabIcon(index).isNull();
    const bool hasText = !tabText(index).isEmpty();
    if (style == Qt::ToolButtonTextBesideIcon || !hasIcon || !hasText)
        return size;                                     // same arrangement as QTabBar's own hint

    const QFontMetrics fm = fontMetrics();
    const int textW = fm.horizontalAdvance(tabText(index));
    const int textH = fm.height();
    const QSize icon = iconSize();
    const int qtSpacing = 4;                             // spacing used by QTabBar's own hint

    switch (style)
    {
    case Qt::ToolButtonIconOnly:
        size.rwidth() -= textW + qtSpacing;
        break;
    case Qt::ToolButtonTextOnly:
        size.rwidth() -= icon.width() + qtSpacing;
        size.rheight() -= qMax(0, icon.height() - textH);
        break;
    case Qt::ToolButtonTextUnderIcon:
        size.rwidth() -= qMin(textW, icon.width()) + qtSpacing;
        size.rheight() += qMin(textH, icon.height()) + m_spacing;
        break;
    default:
        break;
    }
    return size;
}

QSize XTabBar::minimumTabSizeHint(int index) const
{
    if (!usesCustomLayout())
        return QTabBar::minimumTabSizeHint(index);

    // QTabBar's own minimum elides the text with elideMode(); with ElideNone
    // (Fusion, Windows) that is the full text, so tabs could never shrink when
    // scroll buttons are off. Our minimum keeps the icon and an ellipsis.
    QSize size = tabSizeHint(index);
    const QString text = tabText(index);
    const Qt::ToolButtonStyle style = effectiveStyle();
    if (style == Qt::ToolButtonIconOnly || text.isEmpty())
        return size;

    const QFontMetrics fm = fontMetrics();
    const int textW = fm.horizontalAdvance(text);
    const int ellipsisW = fm.horizontalAdvance(QStringLiteral("…"));
    int keep = ellipsisW;                                 // text column shrinks to "..."
    if (style == Qt::ToolButtonTextUnderIcon && !tabIcon(index).isNull())
        keep = qMax(keep, iconSize().width());           // ... but never narrower than the icon
    size.rwidth() -= qMax(0, textW - keep);
    return size;
}

// ---------------------------------------------------------------------------
// Painting
// ---------------------------------------------------------------------------
void XTabBar::paintEvent(QPaintEvent* event)
{
    if (!usesCustomLayout())
    {
        QTabBar::paintEvent(event);
        return;
    }

    QStylePainter painter(this);

    const int current = currentIndex();
    if (drawBase())
    {
        QStyleOptionTabBarBase base;
        base.initFrom(this);
        base.shape = shape();
        base.documentMode = documentMode();
        base.tabBarRect = rect();
        base.selectedTabRect = current >= 0 ? tabRect(current) : QRect();
        painter.drawPrimitive(QStyle::PE_FrameTabBarBase, base);
    }

    // Unselected tabs first, selected one last so it overlaps its neighbours.
    for (int i = 0; i < count(); ++i)
        if (i != current)
            drawTab(painter, i);
    if (current >= 0)
        drawTab(painter, current);
}

void XTabBar::drawTab(QPainter& painter, int index)
{
    QStyleOptionTab option;
    initStyleOption(&option, index);
    if (!option.rect.isValid() || !option.rect.intersects(rect()))
        return;                                          // hidden or scrolled out of view

    // 1. tab shape without any content
    QStyleOptionTab shape = option;
    shape.icon = QIcon();
    shape.text.clear();
    style()->drawControl(QStyle::CE_TabBarTabShape, &shape, &painter, this);

    // 2. icon and text at the positions of the chosen layout
    QRect iconRect, textRect;
    layoutTab(option, index, iconRect, textRect);

    // Never paint content outside the tab: a neighbour painted later would
    // otherwise overwrite overflowing text.
    painter.save();
    painter.setClipRect(option.rect.intersected(rect()));

    if (iconRect.isValid())
    {
        const QIcon::Mode mode = (option.state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled;
        const QIcon::State state = (option.state & QStyle::State_Selected) ? QIcon::On : QIcon::Off;
        tabIcon(index).paint(&painter, iconRect, Qt::AlignCenter, mode, state);
    }

    if (textRect.isValid())
    {
        // Fusion/Windows use ElideNone for tabs; without eliding, long titles
        // would be centered and cut on both sides. Elide on the right instead.
        Qt::TextElideMode elide = elideMode();
        if (elide == Qt::ElideNone)
            elide = Qt::ElideRight;

        // Let the style draw the text (QSS color/font, focus frame) exactly as it
        // would for a tab without icon, i.e. centered in the style's own text
        // rect for the real tab geometry. Then shift the painter so that center
        // lands on the center of our textRect. This works for Fusion as well as
        // for QStyleSheetStyle, which re-reads the real tab rect while drawing.
        QStyleOptionTab label = option;
        label.icon = QIcon();
        label.text = fontMetrics().elidedText(tabText(index), elide, textRect.width());
        const QRect styleText = style()->subElementRect(QStyle::SE_TabBarTabText, &label, this);
        const QPoint from = styleText.isValid() ? styleText.center() : option.rect.center();
        painter.save();
        painter.translate(textRect.center() - from);
        style()->drawControl(QStyle::CE_TabBarTabLabel, &label, &painter, this);
        painter.restore();
    }

    painter.restore();
}

void XTabBar::layoutTab(const QStyleOptionTab& option, int index, QRect& iconRect, QRect& textRect) const
{
    iconRect = QRect();
    textRect = QRect();

    // Content box: what the style would give the label when there is no icon
    // (excludes paddings and the close/side buttons).
    QStyleOptionTab plain = option;
    plain.icon = QIcon();
    QRect content = style()->subElementRect(QStyle::SE_TabBarTabText, &plain, this);
    if (!content.isValid())
        content = option.rect;
    content = content.marginsRemoved(m_padding);         // our extra padding

    const Qt::ToolButtonStyle style = effectiveStyle();
    const bool showIcon = style != Qt::ToolButtonTextOnly && !tabIcon(index).isNull();
    const bool showText = style != Qt::ToolButtonIconOnly && !tabText(index).isEmpty();
    const QSize icon = iconSize();
    const int textH = fontMetrics().height();
    const int textW = fontMetrics().horizontalAdvance(tabText(index));

    if (showIcon && showText && style == Qt::ToolButtonTextUnderIcon)
    {
        const int total = icon.height() + m_spacing + textH;
        const int top = content.center().y() - total / 2;
        iconRect = QRect(content.center().x() - icon.width() / 2, top, icon.width(), icon.height());
        textRect = QRect(content.left(), top + icon.height() + m_spacing, content.width(), textH);
    }
    else if (showIcon && showText)                      // TextBesideIcon
    {
        const int total = icon.width() + m_spacing + qMin(textW, content.width() - icon.width() - m_spacing);
        const int left = qMax(content.left(), content.center().x() - total / 2);
        iconRect = QRect(left, content.center().y() - icon.height() / 2, icon.width(), icon.height());
        textRect = QRect(left + icon.width() + m_spacing, content.top(),
                         content.right() - (left + icon.width() + m_spacing) + 1, content.height());
    }
    else if (showIcon)
    {
        iconRect = QRect(content.center().x() - icon.width() / 2, content.center().y() - icon.height() / 2,
                         icon.width(), icon.height());
    }
    else if (showText)
    {
        textRect = content;
    }
}
