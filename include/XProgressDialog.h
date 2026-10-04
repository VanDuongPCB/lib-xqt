#pragma once

#include <QProgressDialog>
#include "XGlobal.h"

class XQT_EXPORT XProgressDialog : public QProgressDialog
{
    Q_OBJECT
public:
    using QProgressDialog::QProgressDialog;
    ~XProgressDialog() override = default;

    XQT_WIDGET_COMMON(XProgressDialog)
};
