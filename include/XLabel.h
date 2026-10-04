#pragma once

#include <QLabel>
#include "XGlobal.h"

class XQT_EXPORT XLabel : public QLabel
{
    Q_OBJECT
public:
    using QLabel::QLabel;
    ~XLabel() override = default;

    XQT_WIDGET_COMMON(XLabel)
};
