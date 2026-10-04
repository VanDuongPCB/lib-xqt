#pragma once

#include <QStackedWidget>
#include "XGlobal.h"

class XQT_EXPORT XStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    using QStackedWidget::QStackedWidget;
    ~XStackedWidget() override = default;

    XQT_WIDGET_COMMON(XStackedWidget)
};
