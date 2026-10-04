#pragma once

#include <QErrorMessage>
#include "XGlobal.h"

class XQT_EXPORT XErrorMessage : public QErrorMessage
{
    Q_OBJECT
public:
    using QErrorMessage::QErrorMessage;
    ~XErrorMessage() override = default;

    XQT_WIDGET_COMMON(XErrorMessage)
};
