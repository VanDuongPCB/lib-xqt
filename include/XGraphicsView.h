#pragma once

#include <QGraphicsView>
#include "XGlobal.h"

class XQT_EXPORT XGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    using QGraphicsView::QGraphicsView;
    ~XGraphicsView() override = default;

    XQT_WIDGET_COMMON(XGraphicsView)
};
