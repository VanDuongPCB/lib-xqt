#pragma once

#include <QTextBrowser>
#include "XGlobal.h"

class XQT_EXPORT XTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    using QTextBrowser::QTextBrowser;
    ~XTextBrowser() override = default;

    XQT_WIDGET_COMMON(XTextBrowser)
};
