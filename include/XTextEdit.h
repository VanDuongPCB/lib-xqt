#pragma once

#include <QTextEdit>
#include "XGlobal.h"

class XQT_EXPORT XTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    using QTextEdit::QTextEdit;
    ~XTextEdit() override = default;

    XQT_WIDGET_COMMON(XTextEdit)
};
