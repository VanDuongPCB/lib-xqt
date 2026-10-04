#pragma once

#include <QDial>
#include "XGlobal.h"

class XQT_EXPORT XDial : public QDial
{
    Q_OBJECT
public:
    using QDial::QDial;
    ~XDial() override = default;

    XQT_WIDGET_COMMON(XDial)
};
