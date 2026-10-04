#pragma once

#include <QTreeView>
#include "XGlobal.h"

class XQT_EXPORT XTreeView : public QTreeView
{
    Q_OBJECT
public:
    using QTreeView::QTreeView;
    ~XTreeView() override = default;

    XQT_WIDGET_COMMON(XTreeView)
};
