#pragma once

#include <QMargins>
#include <QTabWidget>
#include "XGlobal.h"

class XTabBar;

/*
    XTabWidget: QTabWidget whose tab bar is an XTabBar, so the icon/text layout
    of the tabs can be chosen like a QToolButton:

        tabWidget->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        tabWidget->setTabPaddingLeft(12);      // extra space inside every tab
        tabWidget->setTabBarLeftMargin(16);    // gap between the widget edge and the first tab

    Widgets in the tab row (same height as the tabs, tabs move aside):

        auto* login = new XToolButton;
        tabWidget->setTabBarCornerWidget(login, Qt::TopLeftCorner);

    From a style sheet:
        XTabWidget { qproperty-toolButtonStyle: ToolButtonTextUnderIcon;
                     qproperty-tabPaddingLeft: 12; qproperty-tabBarLeftMargin: 16; }
*/
class XQT_EXPORT XTabWidget : public QTabWidget
{
    Q_OBJECT
    Q_PROPERTY(Qt::ToolButtonStyle toolButtonStyle READ toolButtonStyle WRITE setToolButtonStyle)
    Q_PROPERTY(int iconTextSpacing READ iconTextSpacing WRITE setIconTextSpacing)
    Q_PROPERTY(QMargins tabPadding READ tabPadding WRITE setTabPadding)
    Q_PROPERTY(int tabPaddingLeft READ tabPaddingLeft WRITE setTabPaddingLeft)
    Q_PROPERTY(int tabBarLeftMargin READ tabBarLeftMargin WRITE setTabBarLeftMargin)
public:
    explicit XTabWidget(QWidget* parent = nullptr);
    ~XTabWidget() override = default;

    XTabBar* xTabBar() const;                 // the tab bar, typed

    Qt::ToolButtonStyle toolButtonStyle() const;
    void setToolButtonStyle(Qt::ToolButtonStyle style);

    int iconTextSpacing() const;
    void setIconTextSpacing(int spacing);

    // Padding inside every tab (forwarded to XTabBar)
    QMargins tabPadding() const;
    void setTabPadding(const QMargins& padding);
    void setTabPadding(int left, int top, int right, int bottom);
    int tabPaddingLeft() const;
    void setTabPaddingLeft(int left);

    // Space between the left edge of the widget and the first tab.
    // Uses the top-left corner slot; a later setTabBarCornerWidget(..., TopLeftCorner) replaces it.
    int tabBarLeftMargin() const;
    void setTabBarLeftMargin(int pixels);

    // A widget placed in the tab row, left or right of the tabs, exactly as tall
    // as the tabs and top-aligned with them (QTabWidget::setCornerWidget alone
    // sizes the widget by its own sizeHint and bottom-aligns it). The widget is
    // reparented; pass nullptr to remove it.
    void setTabBarCornerWidget(QWidget* widget, Qt::Corner corner = Qt::TopLeftCorner);
    QWidget* tabBarCornerWidget(Qt::Corner corner = Qt::TopLeftCorner) const;

    XQT_WIDGET_COMMON(XTabWidget)

protected:
    bool event(QEvent* event) override;

private:
    void alignTabRowCornerWidgets();          // top-align the corner hosts with the tab bar

private:
    XTabBar* m_tabBar = nullptr;
    QWidget* m_leftSpacer = nullptr;
    int m_leftMargin = 0;
    QWidget* m_leftCornerHost = nullptr;      // hosts the user's widget
    QWidget* m_rightCornerHost = nullptr;
};
