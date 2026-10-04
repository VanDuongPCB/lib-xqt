#pragma once

#include <QStatusBar>
#include "XGlobal.h"

class XQT_EXPORT XStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    using QStatusBar::QStatusBar;
    ~XStatusBar() override = default;

    XQT_WIDGET_COMMON(XStatusBar)
};
