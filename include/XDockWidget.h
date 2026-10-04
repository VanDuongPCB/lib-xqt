#pragma once

#include <QDockWidget>
#include "XGlobal.h"

class XQT_EXPORT XDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    using QDockWidget::QDockWidget;
    ~XDockWidget() override = default;

    XQT_WIDGET_COMMON(XDockWidget)
};
