#pragma once

#include <QColumnView>
#include "XGlobal.h"

class XQT_EXPORT XColumnView : public QColumnView
{
    Q_OBJECT
public:
    using QColumnView::QColumnView;
    ~XColumnView() override = default;

    XQT_WIDGET_COMMON(XColumnView)
};
