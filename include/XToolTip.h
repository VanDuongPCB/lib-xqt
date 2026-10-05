#pragma once

#include <QFont>
#include <QPalette>
#include <QPoint>
#include <QRect>
#include <QString>

#include "XGlobal.h"

class QWidget;

/*
    XToolTip: the xQt counterpart of QToolTip.

    QToolTip is a static helper, not a widget, so this class is static as well.
    It forwards to QToolTip and adds a few conveniences. The tooltip window is
    created by Qt and keeps the style sheet selector "QToolTip":

        QToolTip { background: ...; }      (css/XToolTip.css in a theme)

    Usage:
        XToolTip::showText(QCursor::pos(), "Saved", widget);
        XToolTip::showText(QCursor::pos(), "Saved", widget, 1500);   // hide after 1.5 s
        XToolTip::showBelow(button, "Click to start");             // under a widget
        XToolTip::hide();
*/
class XQT_EXPORT XToolTip
{
public:
    XToolTip() = delete;

    static void showText(const QPoint& pos, const QString& text, QWidget* widget = nullptr);
    static void showText(const QPoint& pos, const QString& text, QWidget* widget, int msecDisplayTime);
    static void showText(const QPoint& pos, const QString& text, QWidget* widget, const QRect& rect, int msecDisplayTime = -1);

    // Shows the tip just below (or above) a widget, aligned to its left edge.
    static void showBelow(QWidget* widget, const QString& text, int msecDisplayTime = -1);
    static void showAbove(QWidget* widget, const QString& text, int msecDisplayTime = -1);

    static void hide();
    static bool isVisible();
    static QString text();

    static QPalette palette();
    static void setPalette(const QPalette& palette);
    static QFont font();
    static void setFont(const QFont& font);
};
