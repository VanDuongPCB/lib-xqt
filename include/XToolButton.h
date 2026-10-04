#pragma once

#include <QToolButton>
#include "XGlobal.h"

class XQT_EXPORT XToolButton : public QToolButton
{
    Q_OBJECT
public:
    using QToolButton::QToolButton;
    ~XToolButton() override = default;

    XQT_WIDGET_COMMON(XToolButton)
};
