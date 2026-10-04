#pragma once

#include <QMainWindow>
#include "XGlobal.h"

class XQT_EXPORT XMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    using QMainWindow::QMainWindow;
    ~XMainWindow() override = default;

    XQT_WIDGET_COMMON(XMainWindow)
};
