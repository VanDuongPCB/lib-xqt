#pragma once

#include <QSlider>
#include "XGlobal.h"

class XQT_EXPORT XSlider : public QSlider
{
    Q_OBJECT
public:
    using QSlider::QSlider;
    ~XSlider() override = default;

    XQT_WIDGET_COMMON(XSlider)
};
