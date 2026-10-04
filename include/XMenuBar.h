#pragma once

#include <QMenuBar>
#include "XGlobal.h"

class XQT_EXPORT XMenuBar : public QMenuBar
{
    Q_OBJECT
public:
    using QMenuBar::QMenuBar;
    ~XMenuBar() override = default;

    XQT_WIDGET_COMMON(XMenuBar)
};
