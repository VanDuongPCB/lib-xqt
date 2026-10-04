#pragma once

#include <QFontComboBox>
#include "XGlobal.h"

class XQT_EXPORT XFontComboBox : public QFontComboBox
{
    Q_OBJECT
public:
    using QFontComboBox::QFontComboBox;
    ~XFontComboBox() override = default;

    XQT_WIDGET_COMMON(XFontComboBox)
};
