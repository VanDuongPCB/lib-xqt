#pragma once

#include <QWizard>
#include "XGlobal.h"

class XQT_EXPORT XWizard : public QWizard
{
    Q_OBJECT
public:
    using QWizard::QWizard;
    ~XWizard() override = default;

    XQT_WIDGET_COMMON(XWizard)
};
