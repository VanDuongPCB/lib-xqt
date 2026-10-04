#pragma once

#include <QScrollArea>
#include "XGlobal.h"

class XQT_EXPORT XScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    using QScrollArea::QScrollArea;
    ~XScrollArea() override = default;

    XQT_WIDGET_COMMON(XScrollArea)
};
