#pragma once

#include <QCheckBox>
#include "XGlobal.h"

class XQT_EXPORT XCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    using QCheckBox::QCheckBox;
    ~XCheckBox() override = default;

    XQT_WIDGET_COMMON(XCheckBox)
};
