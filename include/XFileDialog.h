#pragma once

#include <QFileDialog>
#include "XGlobal.h"

class XQT_EXPORT XFileDialog : public QFileDialog
{
    Q_OBJECT
public:
    using QFileDialog::QFileDialog;
    ~XFileDialog() override = default;

    XQT_WIDGET_COMMON(XFileDialog)
};
