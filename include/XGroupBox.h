#pragma once

#include <QGroupBox>
#include "XGlobal.h"

class XQT_EXPORT XGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    using QGroupBox::QGroupBox;
    ~XGroupBox() override = default;

    XQT_WIDGET_COMMON(XGroupBox)
};
