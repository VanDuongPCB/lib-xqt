#pragma once

#include <QRadioButton>
#include "XGlobal.h"

class XQT_EXPORT XRadioButton : public QRadioButton
{
    Q_OBJECT
public:
    using QRadioButton::QRadioButton;
    ~XRadioButton() override = default;

    XQT_WIDGET_COMMON(XRadioButton)
};
