#pragma once

#include <QMargins>
#include <QTabBar>
#include "XGlobal.h"

/*
    XTabBar: QTabBar with a configurable icon/text layout, named after
    QToolButton's Qt::ToolButtonStyle:

        Qt::ToolButtonFollowStyle    default QTabBar look (icon left of text)
        Qt::ToolButtonIconOnly       icon only
        Qt::ToolButtonTextOnly       text only
        Qt::ToolButtonTextBesideIcon icon left of text, centered as a group
        Qt::ToolButtonTextUnderIcon  icon above text

    tabBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tabBar->setIconTextSpacing(6);
    tabBar->setTabPaddingLeft(12);                 // extra space inside every tab, left side
    tabBar->setTabPadding(QMargins(12, 4, 12, 4)); // or all four sides

    From a style sheet:
        XTabBar { qproperty-toolButtonStyle: ToolButtonTextUnderIcon; qproperty-tabPaddingLeft: 12; }

    The custom layout applies to horizontal tab bars (RoundedNorth/South,
    TriangularNorth/South). Vertical tab bars keep the default QTabBar painting.
    Tab text color and font still come from the style / style sheet
    (QTabBar::tab { color: ... }).
*/
class XQT_EXPORT XTabBar : public QTabBar
{
    Q_OBJECT
    Q_PROPERTY(Qt::ToolButtonStyle toolButtonStyle READ toolButtonStyle WRITE setToolButtonStyle NOTIFY toolButtonStyleChanged)
    Q_PROPERTY(int iconTextSpacing READ iconTextSpacing WRITE setIconTextSpacing)
    Q_PROPERTY(QMargins tabPadding READ tabPadding WRITE setTabPadding)
    Q_PROPERTY(int tabPaddingLeft READ tabPaddingLeft WRITE setTabPaddingLeft)
    Q_PROPERTY(int tabPaddingTop READ tabPaddingTop WRITE setTabPaddingTop)
    Q_PROPERTY(int tabPaddingRight READ tabPaddingRight WRITE setTabPaddingRight)
    Q_PROPERTY(int tabPaddingBottom READ tabPaddingBottom WRITE setTabPaddingBottom)
public:
    explicit XTabBar(QWidget* parent = nullptr);
    ~XTabBar() override = default;

    Qt::ToolButtonStyle toolButtonStyle() const;
    void setToolButtonStyle(Qt::ToolButtonStyle style);

    int iconTextSpacing() const;              // pixels between icon and text (default 4)
    void setIconTextSpacing(int spacing);

    // Extra padding inside every tab, added to the style's own padding (CSS-like:
    // the tab grows by the padding and the content moves by the same amount, so a
    // left padding of 12 px shifts icon and text 12 px to the right).
    // Setting any padding switches a FollowStyle bar to the custom painting
    // (same look as TextBesideIcon).
    QMargins tabPadding() const;
    void setTabPadding(const QMargins& padding);
    void setTabPadding(int left, int top, int right, int bottom);
    int tabPaddingLeft() const;
    void setTabPaddingLeft(int left);
    int tabPaddingTop() const;
    void setTabPaddingTop(int top);
    int tabPaddingRight() const;
    void setTabPaddingRight(int right);
    int tabPaddingBottom() const;
    void setTabPaddingBottom(int bottom);

    XQT_WIDGET_COMMON(XTabBar)

signals:
    void toolButtonStyleChanged(Qt::ToolButtonStyle style);

protected:
    QSize tabSizeHint(int index) const override;
    QSize minimumTabSizeHint(int index) const override;
    void paintEvent(QPaintEvent* event) override;

private:
    void relayoutTabs();                      // QTabBar caches tab geometry; force it to recompute
    bool usesCustomLayout() const;            // (style != FollowStyle or padding set) and horizontal shape
    Qt::ToolButtonStyle effectiveStyle() const;   // FollowStyle paints like TextBesideIcon
    void drawTab(QPainter& painter, int index);
    void layoutTab(const QStyleOptionTab& option, int index, QRect& iconRect, QRect& textRect) const;

private:
    Qt::ToolButtonStyle m_style = Qt::ToolButtonFollowStyle;
    int m_spacing = 4;
    QMargins m_padding;
};
