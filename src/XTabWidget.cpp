#include "XTabWidget.h"
#include "XTabBar.h"

#include <QEvent>
#include <QPointer>
#include <QResizeEvent>
#include <QStyle>
#include <QTabBar>
#include <QWidget>

namespace
{
    // Height of the tab row as the user sees it: the tab bar's hint minus the
    // part that overlaps the pane frame (PM_TabBarBaseOverlap). QTabWidget puts
    // the bottom of a corner widget on the pane's top edge, so a corner widget
    // with exactly this height is top-aligned with the tabs.
    int tabRowHeight(const QTabBar* bar)
    {
        if (!bar)
            return 0;
        const int overlap = bar->style()->pixelMetric(QStyle::PM_TabBarBaseOverlap, nullptr, bar);
        return qMax(1, bar->sizeHint().height() - overlap);
    }

    // QTabWidget positions corner widgets by their sizeHint(); a plain QWidget
    // reports an invalid hint, so the spacer must provide one.
    class FixedWidthSpacer : public QWidget
    {
    public:
        explicit FixedWidthSpacer(int width, QWidget* parent) : QWidget(parent), m_width(width)
        {
            setAttribute(Qt::WA_TransparentForMouseEvents);
            setFixedWidth(width);
        }
        void setWidth(int width) { m_width = width; setFixedWidth(width); updateGeometry(); }
        QSize sizeHint() const override { return QSize(m_width, 1); }
        QSize minimumSizeHint() const override { return QSize(m_width, 1); }
    private:
        int m_width;
    };

    // Hosts a user widget in the tab row: width from the child, height = tab row.
    // The child is resized to fill the host directly (no layout), so widgets with
    // a Fixed vertical size policy such as QToolButton still get the full height.
    class TabRowCornerHost : public QWidget
    {
    public:
        TabRowCornerHost(QWidget* child, QTabBar* bar, QWidget* parent) : QWidget(parent), m_child(child), m_bar(bar)
        {
            child->setParent(this);
            child->show();
            if (m_bar)
                m_bar->installEventFilter(this);
        }

        QWidget* child() const { return m_child; }

        QSize sizeHint() const override
        {
            const QSize c = m_child ? m_child->sizeHint() : QSize(0, 0);
            return QSize(c.width(), tabRowHeight(m_bar));
        }

        QSize minimumSizeHint() const override
        {
            const QSize c = m_child ? m_child->minimumSizeHint() : QSize(0, 0);
            return QSize(c.width(), tabRowHeight(m_bar));
        }

    protected:
        void resizeEvent(QResizeEvent* event) override
        {
            QWidget::resizeEvent(event);
            if (m_child)
                m_child->setGeometry(rect());
        }

        bool eventFilter(QObject* watched, QEvent* event) override
        {
            // Tabs changed size (style, font, layout mode...): ask QTabWidget to
            // lay us out again. Its LayoutRequest handling calls setUpLayout().
            if (watched == m_bar)
            {
                switch (event->type())
                {
                case QEvent::Resize:
                case QEvent::StyleChange:
                case QEvent::FontChange:
                case QEvent::LayoutRequest:
                    updateGeometry();
                    break;
                default:
                    break;
                }
            }
            return QWidget::eventFilter(watched, event);
        }

    private:
        QPointer<QWidget> m_child;
        QPointer<QTabBar> m_bar;
    };
}

// ---------------------------------------------------------------------------
XTabWidget::XTabWidget(QWidget* parent) : QTabWidget(parent)
{
    m_tabBar = new XTabBar(this);
    setTabBar(m_tabBar);                      // QTabWidget takes ownership
}

XTabBar* XTabWidget::xTabBar() const
{
    return m_tabBar;
}

Qt::ToolButtonStyle XTabWidget::toolButtonStyle() const
{
    return m_tabBar->toolButtonStyle();
}

void XTabWidget::setToolButtonStyle(Qt::ToolButtonStyle style)
{
    m_tabBar->setToolButtonStyle(style);
}

int XTabWidget::iconTextSpacing() const
{
    return m_tabBar->iconTextSpacing();
}

void XTabWidget::setIconTextSpacing(int spacing)
{
    m_tabBar->setIconTextSpacing(spacing);
}

// ---------------------------------------------------------------------------
// Padding
// ---------------------------------------------------------------------------
QMargins XTabWidget::tabPadding() const
{
    return m_tabBar->tabPadding();
}

void XTabWidget::setTabPadding(const QMargins& padding)
{
    m_tabBar->setTabPadding(padding);
}

void XTabWidget::setTabPadding(int left, int top, int right, int bottom)
{
    m_tabBar->setTabPadding(left, top, right, bottom);
}

int XTabWidget::tabPaddingLeft() const
{
    return m_tabBar->tabPaddingLeft();
}

void XTabWidget::setTabPaddingLeft(int left)
{
    m_tabBar->setTabPaddingLeft(left);
}

// ---------------------------------------------------------------------------
// Left margin (spacer in the top-left corner slot)
// ---------------------------------------------------------------------------
int XTabWidget::tabBarLeftMargin() const
{
    return m_leftMargin;
}

void XTabWidget::setTabBarLeftMargin(int pixels)
{
    pixels = qMax(0, pixels);
    if (m_leftMargin == pixels)
        return;
    m_leftMargin = pixels;

    if (pixels == 0)
    {
        if (m_leftSpacer)
        {
            if (cornerWidget(Qt::TopLeftCorner) == m_leftSpacer)
                setCornerWidget(nullptr, Qt::TopLeftCorner);
            delete m_leftSpacer;
            m_leftSpacer = nullptr;
        }
        return;
    }

    if (!m_leftSpacer)
        m_leftSpacer = new FixedWidthSpacer(pixels, this);
    else
        static_cast<FixedWidthSpacer*>(m_leftSpacer)->setWidth(pixels);
    setCornerWidget(m_leftSpacer, Qt::TopLeftCorner);   // (re)runs QTabWidget's layout
    m_leftSpacer->show();
}

// ---------------------------------------------------------------------------
// Widgets in the tab row
// ---------------------------------------------------------------------------
void XTabWidget::setTabBarCornerWidget(QWidget* widget, Qt::Corner corner)
{
    const bool left = (corner == Qt::TopLeftCorner || corner == Qt::BottomLeftCorner);
    QWidget*& host = left ? m_leftCornerHost : m_rightCornerHost;
    const Qt::Corner slot = left ? Qt::TopLeftCorner : Qt::TopRightCorner;

    if (host)
    {
        if (cornerWidget(slot) == host)
            setCornerWidget(nullptr, slot);
        delete host;                          // deletes the previous child as well
        host = nullptr;
    }
    if (left && m_leftSpacer && m_leftMargin > 0)
        m_leftMargin = 0;                     // the margin spacer gives way to the widget

    if (!widget)
    {
        setCornerWidget(nullptr, slot);
        return;
    }

    host = new TabRowCornerHost(widget, m_tabBar, this);
    setCornerWidget(host, slot);
    host->show();
}

QWidget* XTabWidget::tabBarCornerWidget(Qt::Corner corner) const
{
    const bool left = (corner == Qt::TopLeftCorner || corner == Qt::BottomLeftCorner);
    QWidget* host = left ? m_leftCornerHost : m_rightCornerHost;
    return host ? static_cast<TabRowCornerHost*>(host)->child() : nullptr;
}

bool XTabWidget::event(QEvent* event)
{
    const bool handled = QTabWidget::event(event);

    // QTabWidget::setUpLayout() runs on these events and places the corner
    // widgets by their size hint, bottom-aligned to the pane. Fix them up right
    // after so they share the tab bar's top edge and height.
    switch (event->type())
    {
    case QEvent::Resize:
    case QEvent::Show:
    case QEvent::LayoutRequest:
    case QEvent::StyleChange:
    case QEvent::FontChange:
    case QEvent::Polish:
        alignTabRowCornerWidgets();
        break;
    default:
        break;
    }
    return handled;
}

void XTabWidget::alignTabRowCornerWidgets()
{
    if (!m_tabBar || (!m_leftCornerHost && !m_rightCornerHost))
        return;

    const QRect bar = m_tabBar->geometry();
    if (!bar.isValid())
        return;

    const int height = tabRowHeight(m_tabBar);
    if (m_leftCornerHost)
    {
        const int w = m_leftCornerHost->width();
        m_leftCornerHost->setGeometry(bar.left() - w, bar.top(), w, height);
    }
    if (m_rightCornerHost)
    {
        const int w = m_rightCornerHost->width();
        m_rightCornerHost->setGeometry(bar.right() + 1, bar.top(), w, height);
    }
}
