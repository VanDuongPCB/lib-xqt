#pragma once

#include <QInputDialog>
#include "XGlobal.h"

class XQT_EXPORT XInputDialog : public QInputDialog
{
    Q_OBJECT
public:
    using QInputDialog::QInputDialog;
    ~XInputDialog() override = default;

    XQT_WIDGET_COMMON(XInputDialog)
};
