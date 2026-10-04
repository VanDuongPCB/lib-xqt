#pragma once

#include <QKeySequenceEdit>
#include "XGlobal.h"

class XQT_EXPORT XKeySequenceEdit : public QKeySequenceEdit
{
    Q_OBJECT
public:
    using QKeySequenceEdit::QKeySequenceEdit;
    ~XKeySequenceEdit() override = default;

    XQT_WIDGET_COMMON(XKeySequenceEdit)
};
