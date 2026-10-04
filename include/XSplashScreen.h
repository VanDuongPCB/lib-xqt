#pragma once

#include <QSplashScreen>
#include "XGlobal.h"

class XQT_EXPORT XSplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    using QSplashScreen::QSplashScreen;
    ~XSplashScreen() override = default;

    XQT_WIDGET_COMMON(XSplashScreen)
};
