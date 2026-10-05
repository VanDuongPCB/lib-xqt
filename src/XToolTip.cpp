#include "XToolTip.h"

#include <QToolTip>
#include <QWidget>

void XToolTip::showText(const QPoint& pos, const QString& text, QWidget* widget)
{
    QToolTip::showText(pos, text, widget);
}

void XToolTip::showText(const QPoint& pos, const QString& text, QWidget* widget, int msecDisplayTime)
{
    QToolTip::showText(pos, text, widget, QRect(), msecDisplayTime);
}

void XToolTip::showText(const QPoint& pos, const QString& text, QWidget* widget, const QRect& rect, int msecDisplayTime)
{
    QToolTip::showText(pos, text, widget, rect, msecDisplayTime);
}

void XToolTip::showBelow(QWidget* widget, const QString& text, int msecDisplayTime)
{
    if (!widget)
        return;
    const QPoint pos = widget->mapToGlobal(QPoint(0, widget->height()));
    QToolTip::showText(pos, text, widget, QRect(), msecDisplayTime);
}

void XToolTip::showAbove(QWidget* widget, const QString& text, int msecDisplayTime)
{
    if (!widget)
        return;
    // QToolTip places the tip below the point; move up by roughly one tip height.
    const QPoint pos = widget->mapToGlobal(QPoint(0, -widget->fontMetrics().height() * 2 - 12));
    QToolTip::showText(pos, text, widget, QRect(), msecDisplayTime);
}

void XToolTip::hide()
{
    QToolTip::hideText();
}

bool XToolTip::isVisible()
{
    return QToolTip::isVisible();
}

QString XToolTip::text()
{
    return QToolTip::text();
}

QPalette XToolTip::palette()
{
    return QToolTip::palette();
}

void XToolTip::setPalette(const QPalette& palette)
{
    QToolTip::setPalette(palette);
}

QFont XToolTip::font()
{
    return QToolTip::font();
}

void XToolTip::setFont(const QFont& font)
{
    QToolTip::setFont(font);
}
