#pragma once

#include <QPlainTextEdit>
#include "XGlobal.h"

class XQT_EXPORT XPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    using QPlainTextEdit::QPlainTextEdit;
    ~XPlainTextEdit() override = default;

    XQT_WIDGET_COMMON(XPlainTextEdit)
};
