#pragma once

#include <QFrame>
#include "XGlobal.h"

class XQT_EXPORT XFrame : public QFrame
{
    Q_OBJECT
public:
    using QFrame::QFrame;
    ~XFrame() override = default;

    XQT_WIDGET_COMMON(XFrame)
};
