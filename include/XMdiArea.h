#pragma once

#include <QMdiArea>
#include "XGlobal.h"

class XQT_EXPORT XMdiArea : public QMdiArea
{
    Q_OBJECT
public:
    using QMdiArea::QMdiArea;
    ~XMdiArea() override = default;

    XQT_WIDGET_COMMON(XMdiArea)
};
