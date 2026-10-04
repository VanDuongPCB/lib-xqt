#pragma once

#include <QTableWidget>
#include "XGlobal.h"

class XQT_EXPORT XTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    using QTableWidget::QTableWidget;
    ~XTableWidget() override = default;

    XQT_WIDGET_COMMON(XTableWidget)
};
