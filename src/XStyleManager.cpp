#include "XStyleManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointer>
#include <QRegularExpression>
#include <QTimer>
#include <QWidget>
#include <QApplication>

#include <algorithm>

namespace
{
    const QStringList kStyleSheetSuffixes = { "css", "qss" };
    const QStringList kVariableFiles = { "variables.json", "colors.json" };
    const int kReloadDebounceMs = 150;

    QString toPosix(const QString& path)
    {
        return QDir::fromNativeSeparators(QDir::cleanPath(path));
    }
}

// ---------------------------------------------------------------------------
// Construction / instance
// ---------------------------------------------------------------------------
XStyleManager::XStyleManager(QObject* parent) : QObject(parent)
{
}

XStyleManager::~XStyleManager() = default;

XStyleManager* XStyleManager::instance()
{
    static QPointer<XStyleManager> s_instance;
    if (!s_instance)
        s_instance = new XStyleManager(QCoreApplication::instance());
    return s_instance;
}

// ---------------------------------------------------------------------------
// Theme root and discovery
// ---------------------------------------------------------------------------
void XStyleManager::setThemeRoot(const QString& dir)
{
    m_themeRoot = toPosix(dir);
}

QString XStyleManager::themeRoot() const
{
    return m_themeRoot;
}

QStringList XStyleManager::availableThemes() const
{
    QStringList names;
    if (m_themeRoot.isEmpty())
        return names;

    const QFileInfoList entries = QDir(m_themeRoot).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QFileInfo& info : entries)
    {
        if (QDir(info.absoluteFilePath() + "/css").exists())
            names.push_back(info.fileName());
    }
    return names;
}

// ---------------------------------------------------------------------------
// Loading
// ---------------------------------------------------------------------------
bool XStyleManager::loadTheme(const QString& name)
{
    if (!readTheme(name))
        return false;

    updateWatcher();
    if (m_autoApply)
        applyToApplication();

    emit themeChanged(m_themeName);
    emit styleSheetChanged();
    return true;
}

bool XStyleManager::reload()
{
    if (m_themeName.isEmpty())
    {
        setError("reload(): no theme loaded");
        return false;
    }

    const QString before = m_combined;
    if (!readTheme(m_themeName))
        return false;

    updateWatcher();
    if (m_combined != before)
    {
        if (m_autoApply)
            applyToApplication();
        emit styleSheetChanged();
    }
    return true;
}

QString XStyleManager::currentTheme() const
{
    return m_themeName;
}

QString XStyleManager::themeDir() const
{
    return m_themeDir;
}

QString XStyleManager::imageDir() const
{
    return m_themeDir.isEmpty() ? QString() : m_themeDir + "/images";
}

bool XStyleManager::isLoaded() const
{
    return !m_themeName.isEmpty();
}

bool XStyleManager::readTheme(const QString& name)
{
    if (m_themeRoot.isEmpty())
    {
        setError("theme root is not set (call setThemeRoot first)");
        return false;
    }

    const QString dir = m_themeRoot + "/" + name;
    if (!QDir(dir).exists())
    {
        setError(QString("theme \"%1\" not found in %2").arg(name, m_themeRoot));
        return false;
    }

    QVariantMap fileVars;
    readVariables(dir + "/data", fileVars);        // optional, errors are reported but not fatal

    QStringList names, contents;
    if (!readStyleSheets(dir + "/css", names, contents))
        return false;

    // Commit the new state, then substitute (substitute() reads members).
    m_themeName = name;
    m_themeDir = dir;
    m_fileVariables = fileVars;
    m_sheetNames = names;
    m_sheetContents.clear();
    for (const QString& css : contents)
        m_sheetContents.push_back(substitute(css));
    m_combined = m_sheetContents.join("\n");
    m_lastError.clear();
    return true;
}

bool XStyleManager::readVariables(const QString& dir, QVariantMap& out)
{
    out.clear();
    for (const QString& fileName : kVariableFiles)
    {
        QFile file(dir + "/" + fileName);
        if (!file.exists())
            continue;
        if (!file.open(QIODevice::ReadOnly))
        {
            setError(QString("cannot open %1").arg(file.fileName()));
            return false;
        }
        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isObject())
        {
            setError(QString("%1: %2").arg(file.fileName(), parseError.errorString()));
            return false;
        }
        const QJsonObject obj = doc.object();
        for (auto it = obj.begin(); it != obj.end(); ++it)
            out.insert(it.key(), it.value().toVariant());
        return true;
    }
    return true;   // no variables file: fine
}

bool XStyleManager::readStyleSheets(const QString& dir, QStringList& names, QStringList& contents)
{
    names.clear();
    contents.clear();

    QDir cssDir(dir);
    if (!cssDir.exists())
    {
        setError(QString("css directory not found: %1").arg(dir));
        return false;
    }

    QFileInfoList files = cssDir.entryInfoList(QDir::Files, QDir::NoSort);
    files.erase(std::remove_if(files.begin(), files.end(), [](const QFileInfo& fi) {
                    return !kStyleSheetSuffixes.contains(fi.suffix().toLower());
                }),
                files.end());
    std::sort(files.begin(), files.end(), [](const QFileInfo& a, const QFileInfo& b) {
        return QString::compare(a.fileName(), b.fileName(), Qt::CaseInsensitive) < 0;
    });

    for (const QFileInfo& fi : files)
    {
        QFile file(fi.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            setError(QString("cannot open %1").arg(fi.absoluteFilePath()));
            return false;
        }
        names.push_back(fi.completeBaseName());
        contents.push_back(QString::fromUtf8(file.readAll()));
    }
    return true;
}

QString XStyleManager::substitute(const QString& css) const
{
    static const QRegularExpression token(R"(@([A-Za-z_][A-Za-z0-9_\-]*))");

    const QVariantMap vars = variables();
    QString result;
    result.reserve(css.size());

    int last = 0;
    QRegularExpressionMatchIterator it = token.globalMatch(css);
    while (it.hasNext())
    {
        const QRegularExpressionMatch m = it.next();
        const QString key = m.captured(1);
        const auto found = vars.constFind(key);
        if (found == vars.constEnd())
            continue;                                     // unknown token: keep as is
        result += css.mid(last, m.capturedStart(0) - last);
        result += found->toString();
        last = m.capturedEnd(0);
    }
    result += css.mid(last);
    return result;
}

// ---------------------------------------------------------------------------
// Variables
// ---------------------------------------------------------------------------
void XStyleManager::setVariable(const QString& key, const QString& value)
{
    m_userVariables.insert(key, value);
}

void XStyleManager::removeVariable(const QString& key)
{
    m_userVariables.remove(key);
}

QString XStyleManager::variable(const QString& key, const QString& fallback) const
{
    const QVariantMap vars = variables();
    const auto it = vars.constFind(key);
    return it == vars.constEnd() ? fallback : it->toString();
}

QColor XStyleManager::color(const QString& key, const QColor& fallback) const
{
    const QString value = variable(key);
    if (value.isEmpty())
        return fallback;
    const QColor c(value);
    return c.isValid() ? c : fallback;
}

QVariantMap XStyleManager::variables() const
{
    QVariantMap vars;
    vars.insert("themeName", m_themeName);
    vars.insert("themeDir", m_themeDir);
    vars.insert("imageDir", imageDir());
    for (auto it = m_fileVariables.cbegin(); it != m_fileVariables.cend(); ++it)
        vars.insert(it.key(), it.value());
    for (auto it = m_userVariables.cbegin(); it != m_userVariables.cend(); ++it)
        vars.insert(it.key(), it.value());
    return vars;
}

// ---------------------------------------------------------------------------
// Style sheets
// ---------------------------------------------------------------------------
QString XStyleManager::styleSheet() const
{
    return m_combined;
}

QString XStyleManager::styleSheet(const QStringList& names) const
{
    if (names.isEmpty())
        return m_combined;

    QStringList parts;
    for (const QString& name : names)
    {
        const int index = m_sheetNames.indexOf(name);
        if (index >= 0)
            parts.push_back(m_sheetContents.at(index));
    }
    return parts.join("\n");
}

QStringList XStyleManager::styleSheetNames() const
{
    return m_sheetNames;
}

// ---------------------------------------------------------------------------
// Applying
// ---------------------------------------------------------------------------
void XStyleManager::setAutoApply(bool enabled)
{
    m_autoApply = enabled;
}

bool XStyleManager::autoApply() const
{
    return m_autoApply;
}

void XStyleManager::applyToApplication() const
{
    if (auto* app = qobject_cast<QApplication*>(QCoreApplication::instance()))
        app->setStyleSheet(m_combined);
}

void XStyleManager::applyTo(QWidget* widget, const QStringList& names) const
{
    if (widget)
        widget->setStyleSheet(styleSheet(names));
}

// ---------------------------------------------------------------------------
// Hot reload
// ---------------------------------------------------------------------------
void XStyleManager::setWatchEnabled(bool enabled)
{
    if (m_watchEnabled == enabled)
        return;
    m_watchEnabled = enabled;

    if (enabled && !m_watcher)
    {
        m_watcher = new QFileSystemWatcher(this);
        m_reloadTimer = new QTimer(this);
        m_reloadTimer->setSingleShot(true);
        m_reloadTimer->setInterval(kReloadDebounceMs);
        connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &XStyleManager::onWatchedPathChanged);
        connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &XStyleManager::onWatchedPathChanged);
        connect(m_reloadTimer, &QTimer::timeout, this, [this]() { reload(); });
    }
    updateWatcher();
}

bool XStyleManager::isWatchEnabled() const
{
    return m_watchEnabled;
}

void XStyleManager::updateWatcher()
{
    if (!m_watcher)
        return;

    const QStringList oldFiles = m_watcher->files();
    const QStringList oldDirs = m_watcher->directories();
    if (!oldFiles.isEmpty())
        m_watcher->removePaths(oldFiles);
    if (!oldDirs.isEmpty())
        m_watcher->removePaths(oldDirs);

    if (!m_watchEnabled || m_themeDir.isEmpty())
        return;

    QStringList paths;
    for (const QString& sub : { QString("/css"), QString("/data") })
    {
        const QString dir = m_themeDir + sub;
        if (!QDir(dir).exists())
            continue;
        paths.push_back(dir);
        const QFileInfoList files = QDir(dir).entryInfoList(QDir::Files);
        for (const QFileInfo& fi : files)
            paths.push_back(fi.absoluteFilePath());
    }
    if (!paths.isEmpty())
        m_watcher->addPaths(paths);
}

void XStyleManager::onWatchedPathChanged(const QString&)
{
    // Editors often delete and recreate files; debounce and re-read everything.
    if (m_reloadTimer)
        m_reloadTimer->start();
}

// ---------------------------------------------------------------------------
// Errors
// ---------------------------------------------------------------------------
QString XStyleManager::lastError() const
{
    return m_lastError;
}

void XStyleManager::setError(const QString& message)
{
    m_lastError = message;
    emit errorOccurred(message);
}
