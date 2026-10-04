#pragma once

#include <QSpinBox>
#include "XGlobal.h"

class XQT_EXPORT XSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    using QSpinBox::QSpinBox;
    ~XSpinBox() override = default;

    XQT_WIDGET_COMMON(XSpinBox)
};
