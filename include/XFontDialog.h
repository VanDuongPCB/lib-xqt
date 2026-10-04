#pragma once

#include <QFontDialog>
#include "XGlobal.h"

class XQT_EXPORT XFontDialog : public QFontDialog
{
    Q_OBJECT
public:
    using QFontDialog::QFontDialog;
    ~XFontDialog() override = default;

    XQT_WIDGET_COMMON(XFontDialog)
};
