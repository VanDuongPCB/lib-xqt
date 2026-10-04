#include "XGlobal.h"

#include <QStyle>
#include <QVariant>
#include <QWidget>

namespace
{
    const char* const kStyleClassProperty = "class";
}

namespace xQt
{
    QString version()
    {
        return QStringLiteral(XQT_VERSION_STR);
    }

    void repolish(QWidget* widget)
    {
        if (!widget)
            return;
        if (QStyle* style = widget->style())
        {
            style->unpolish(widget);
            style->polish(widget);
        }
        widget->update();
    }

    QStringList styleClasses(const QWidget* widget)
    {
        if (!widget)
            return {};
        return widget->property(kStyleClassProperty).toString().split(' ', Qt::SkipEmptyParts);
    }

    void setStyleClasses(QWidget* widget, const QStringList& classes, bool apply)
    {
        if (!widget)
            return;
        QStringList cleaned;
        for (const QString& c : classes)
        {
            const QString t = c.trimmed();
            if (!t.isEmpty() && !cleaned.contains(t))
                cleaned.push_back(t);
        }
        const QString joined = cleaned.join(' ');
        if (widget->property(kStyleClassProperty).toString() == joined)
            return;
        widget->setProperty(kStyleClassProperty, joined);
        if (apply)
            repolish(widget);
    }

    void addStyleClass(QWidget* widget, const QString& styleClass, bool apply)
    {
        QStringList classes = styleClasses(widget);
        if (classes.contains(styleClass.trimmed()))
            return;
        classes.push_back(styleClass.trimmed());
        setStyleClasses(widget, classes, apply);
    }

    void removeStyleClass(QWidget* widget, const QString& styleClass, bool apply)
    {
        QStringList classes = styleClasses(widget);
        if (!classes.removeAll(styleClass.trimmed()))
            return;
        setStyleClasses(widget, classes, apply);
    }

    bool hasStyleClass(const QWidget* widget, const QString& styleClass)
    {
        return styleClasses(widget).contains(styleClass.trimmed());
    }
}
