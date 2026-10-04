#pragma once

#include <QTabBar>
#include "XGlobal.h"

class XQT_EXPORT XTabBar : public QTabBar
{
    Q_OBJECT
public:
    using QTabBar::QTabBar;
    ~XTabBar() override = default;

    XQT_WIDGET_COMMON(XTabBar)
};
