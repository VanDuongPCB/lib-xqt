#pragma once

#include <QProgressBar>
#include "XGlobal.h"

class XQT_EXPORT XProgressBar : public QProgressBar
{
    Q_OBJECT
public:
    using QProgressBar::QProgressBar;
    ~XProgressBar() override = default;

    XQT_WIDGET_COMMON(XProgressBar)
};
