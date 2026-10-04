#pragma once

#include <QDialog>
#include "XGlobal.h"

class XQT_EXPORT XDialog : public QDialog
{
    Q_OBJECT
public:
    using QDialog::QDialog;
    ~XDialog() override = default;

    XQT_WIDGET_COMMON(XDialog)
};
