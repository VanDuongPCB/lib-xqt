#pragma once

#include <QLineEdit>
#include "XGlobal.h"

class XQT_EXPORT XLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    using QLineEdit::QLineEdit;
    ~XLineEdit() override = default;

    XQT_WIDGET_COMMON(XLineEdit)
};
