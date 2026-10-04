#pragma once

#include <QUndoView>
#include "XGlobal.h"

class XQT_EXPORT XUndoView : public QUndoView
{
    Q_OBJECT
public:
    using QUndoView::QUndoView;
    ~XUndoView() override = default;

    XQT_WIDGET_COMMON(XUndoView)
};
