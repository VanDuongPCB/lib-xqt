#pragma once

#include <QTabWidget>
#include "XGlobal.h"

class XQT_EXPORT XTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    using QTabWidget::QTabWidget;
    ~XTabWidget() override = default;

    XQT_WIDGET_COMMON(XTabWidget)
};
