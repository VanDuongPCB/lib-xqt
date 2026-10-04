#pragma once

#include <QTimeEdit>
#include "XGlobal.h"

class XQT_EXPORT XTimeEdit : public QTimeEdit
{
    Q_OBJECT
public:
    using QTimeEdit::QTimeEdit;
    ~XTimeEdit() override = default;

    XQT_WIDGET_COMMON(XTimeEdit)
};
