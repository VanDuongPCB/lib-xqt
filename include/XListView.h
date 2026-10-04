#pragma once

#include <QListView>
#include "XGlobal.h"

class XQT_EXPORT XListView : public QListView
{
    Q_OBJECT
public:
    using QListView::QListView;
    ~XListView() override = default;

    XQT_WIDGET_COMMON(XListView)
};
