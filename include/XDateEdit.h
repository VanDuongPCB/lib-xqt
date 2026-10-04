#pragma once

#include <QDateEdit>
#include "XGlobal.h"

class XQT_EXPORT XDateEdit : public QDateEdit
{
    Q_OBJECT
public:
    using QDateEdit::QDateEdit;
    ~XDateEdit() override = default;

    XQT_WIDGET_COMMON(XDateEdit)
};
