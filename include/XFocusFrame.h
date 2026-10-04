#pragma once

#include <QFocusFrame>
#include "XGlobal.h"

class XQT_EXPORT XFocusFrame : public QFocusFrame
{
    Q_OBJECT
public:
    using QFocusFrame::QFocusFrame;
    ~XFocusFrame() override = default;

    XQT_WIDGET_COMMON(XFocusFrame)
};
