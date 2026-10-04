#pragma once

#include <QMessageBox>
#include "XGlobal.h"

class XQT_EXPORT XMessageBox : public QMessageBox
{
    Q_OBJECT
public:
    using QMessageBox::QMessageBox;
    ~XMessageBox() override = default;

    XQT_WIDGET_COMMON(XMessageBox)
};
