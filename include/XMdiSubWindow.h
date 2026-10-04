#pragma once

#include <QMdiSubWindow>
#include "XGlobal.h"

class XQT_EXPORT XMdiSubWindow : public QMdiSubWindow
{
    Q_OBJECT
public:
    using QMdiSubWindow::QMdiSubWindow;
    ~XMdiSubWindow() override = default;

    XQT_WIDGET_COMMON(XMdiSubWindow)
};
