#pragma once

#include <QPushButton>
#include "XGlobal.h"

class XQT_EXPORT XPushButton : public QPushButton
{
    Q_OBJECT
public:
    using QPushButton::QPushButton;
    ~XPushButton() override = default;

    XQT_WIDGET_COMMON(XPushButton)
};
