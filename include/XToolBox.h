#pragma once

#include <QToolBox>
#include "XGlobal.h"

class XQT_EXPORT XToolBox : public QToolBox
{
    Q_OBJECT
public:
    using QToolBox::QToolBox;
    ~XToolBox() override = default;

    XQT_WIDGET_COMMON(XToolBox)
};
