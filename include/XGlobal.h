#pragma once

#include <QtCore/qglobal.h>
#include <QString>
#include <QStringList>

class QWidget;

// ---------------------------------------------------------------------------
// Export macro
//   static library (default) : XQT_STATIC is defined for users and the library
//   shared library            : XQT_LIBRARY is defined while building xQt
// ---------------------------------------------------------------------------
#if defined(XQT_STATIC)
#  define XQT_EXPORT
#elif defined(XQT_LIBRARY)
#  define XQT_EXPORT Q_DECL_EXPORT
#else
#  define XQT_EXPORT Q_DECL_IMPORT
#endif

namespace xQt
{
    // Library version, e.g. "0.1.0".
    XQT_EXPORT QString version();

    // Re-apply the style sheet to a widget after a dynamic property changed.
    XQT_EXPORT void repolish(QWidget* widget);

    // "Style classes": a space separated list stored in the dynamic property
    // "class", so a style sheet can select widgets with
    //     XPushButton[class~="primary"] { ... }
    // apply = true re-applies the style sheet right away (repolish). Pass false
    // when changing several classes in a row, then call repolish() once.
    XQT_EXPORT QStringList styleClasses(const QWidget* widget);
    XQT_EXPORT void setStyleClasses(QWidget* widget, const QStringList& classes, bool apply = true);
    XQT_EXPORT void addStyleClass(QWidget* widget, const QString& styleClass, bool apply = true);
    XQT_EXPORT void removeStyleClass(QWidget* widget, const QString& styleClass, bool apply = true);
    XQT_EXPORT bool hasStyleClass(const QWidget* widget, const QString& styleClass);
}

// Common API added to every X* widget. Kept as a macro because each wrapper
// needs its own Q_OBJECT and therefore cannot share a template base.
#define XQT_WIDGET_COMMON(ClassName)                                                      \
public:                                                                                   \
    QStringList styleClasses() const { return xQt::styleClasses(this); }                  \
    void setStyleClasses(const QStringList& classes, bool apply = true) { xQt::setStyleClasses(this, classes, apply); } \
    void addStyleClass(const QString& styleClass, bool apply = true) { xQt::addStyleClass(this, styleClass, apply); }   \
    void removeStyleClass(const QString& styleClass, bool apply = true) { xQt::removeStyleClass(this, styleClass, apply); } \
    bool hasStyleClass(const QString& styleClass) const { return xQt::hasStyleClass(this, styleClass); } \
    void repolish() { xQt::repolish(this); }                                              \
private:
