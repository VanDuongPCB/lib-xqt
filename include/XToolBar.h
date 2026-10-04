#pragma once

#include <QToolBar>
#include "XGlobal.h"

class XQT_EXPORT XToolBar : public QToolBar
{
    Q_OBJECT
public:
    using QToolBar::QToolBar;
    ~XToolBar() override = default;

    XQT_WIDGET_COMMON(XToolBar)
};
