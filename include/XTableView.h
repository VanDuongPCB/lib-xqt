#pragma once

#include <QTableView>
#include "XGlobal.h"

class XQT_EXPORT XTableView : public QTableView
{
    Q_OBJECT
public:
    using QTableView::QTableView;
    ~XTableView() override = default;

    XQT_WIDGET_COMMON(XTableView)
};
