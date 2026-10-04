#pragma once

#include <QHeaderView>
#include "XGlobal.h"

class XQT_EXPORT XHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    using QHeaderView::QHeaderView;
    ~XHeaderView() override = default;

    XQT_WIDGET_COMMON(XHeaderView)
};
