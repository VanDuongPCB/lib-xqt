#pragma once

#include <QCommandLinkButton>
#include "XGlobal.h"

class XQT_EXPORT XCommandLinkButton : public QCommandLinkButton
{
    Q_OBJECT
public:
    using QCommandLinkButton::QCommandLinkButton;
    ~XCommandLinkButton() override = default;

    XQT_WIDGET_COMMON(XCommandLinkButton)
};
