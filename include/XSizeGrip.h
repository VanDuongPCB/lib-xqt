#pragma once

#include <QSizeGrip>
#include "XGlobal.h"

class XQT_EXPORT XSizeGrip : public QSizeGrip
{
    Q_OBJECT
public:
    using QSizeGrip::QSizeGrip;
    ~XSizeGrip() override = default;

    XQT_WIDGET_COMMON(XSizeGrip)
};
