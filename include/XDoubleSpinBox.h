#pragma once

#include <QDoubleSpinBox>
#include "XGlobal.h"

class XQT_EXPORT XDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    using QDoubleSpinBox::QDoubleSpinBox;
    ~XDoubleSpinBox() override = default;

    XQT_WIDGET_COMMON(XDoubleSpinBox)
};
