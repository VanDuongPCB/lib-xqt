#include "XDebug.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMutex>
#include <QMutexLocker>
#include <QTextStream>

namespace
{
    // Loaded once; immutable afterwards except the two override fields.
    struct State
    {
        bool initialized = false;
        QString fileName;
        QStringList searchPaths;
        QString filePath;
        QVariantMap options;
        QString lastError;
        bool enabled = false;

        bool hasForced = false;
        bool forcedEnabled = false;
        QString logFileOverride;
    };

    State& state()
    {
        static State s;
        return s;
    }

    QMutex s_initMutex;
    QMutex s_logMutex;
    QtMessageHandler s_previousHandler = nullptr;
    bool s_handlerInstalled = false;

    QString toPosix(const QString& path)
    {
        return QDir::fromNativeSeparators(QDir::cleanPath(path));
    }

    QString normalizeKey(const QString& key)
    {
        return key.trimmed().toLower();
    }

    bool isTruthy(const QVariant& v)
    {
        switch (v.typeId())
        {
        case QMetaType::Bool:
            return v.toBool();
        case QMetaType::Int:
        case QMetaType::LongLong:
        case QMetaType::Double:
            return v.toDouble() != 0.0;
        default:
            break;
        }
        const QString s = v.toString().trimmed().toLower();
        return s == "1" || s == "true" || s == "on" || s == "yes" || s == "enable" || s == "enabled";
    }

    void flatten(const QJsonObject& obj, const QString& prefix, QVariantMap& out)
    {
        for (auto it = obj.begin(); it != obj.end(); ++it)
        {
            const QString key = prefix.isEmpty() ? normalizeKey(it.key()) : prefix + "." + normalizeKey(it.key());
            if (it.value().isObject())
                flatten(it.value().toObject(), key, out);
            else
                out.insert(key, it.value().toVariant());
        }
    }

    const char* typeName(QtMsgType type)
    {
        switch (type)
        {
        case QtDebugMsg:    return "DEBUG";
        case QtInfoMsg:     return "INFO";
        case QtWarningMsg:  return "WARN";
        case QtCriticalMsg: return "ERROR";
        case QtFatalMsg:    return "FATAL";
        }
        return "?";
    }

    QString findFile(const QStringList& dirs, const QString& fileName)
    {
        for (const QString& dir : dirs)
        {
            const QFileInfo fi(dir + "/" + fileName);
            if (fi.exists() && fi.isFile())
                return toPosix(fi.absoluteFilePath());
        }
        return QString();
    }

    QVariantMap parseOptions(const QString& filePath, QString& error)
    {
        QVariantMap options;
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            error = QString("cannot open %1").arg(filePath);
            return options;
        }

        const QByteArray data = file.readAll().trimmed();
        if (data.isEmpty())
            return options;                                   // empty file: enabled, no options

        QJsonParseError err;
        const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error != QJsonParseError::NoError)
        {
            error = QString("%1: %2 at offset %3").arg(filePath, err.errorString()).arg(err.offset);
            return options;
        }
        if (!doc.isObject())
        {
            error = QString("%1: top level must be a JSON object").arg(filePath);
            return options;
        }

        flatten(doc.object(), QString(), options);
        return options;
    }

    void ensureInitialized()
    {
        if (!state().initialized)
            XDebug::initialize();
    }

    const QVariant* findOption(const QString& key)
    {
        ensureInitialized();
        const State& s = state();
        if (!(s.hasForced ? s.forcedEnabled : s.enabled))
            return nullptr;
        const auto it = s.options.constFind(normalizeKey(key));
        return it == s.options.constEnd() ? nullptr : &*it;
    }

    void appendToLog(const QString& line)
    {
        const QString path = XDebug::logFile();
        if (path.isEmpty())
            return;

        QMutexLocker lock(&s_logMutex);
        QFile file(path);
        if (!file.open(QIODevice::Append | QIODevice::Text))
            return;
        QTextStream out(&file);
        out << line << '\n';
    }

    void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
    {
        if (XDebug::isEnabled())
        {
            QString where;
            if (context.file && context.line > 0)
                where = QString(" (%1:%2)").arg(QFileInfo(context.file).fileName()).arg(context.line);
            appendToLog(QString("[%1] [%2] %3%4")
                            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"),
                                 typeName(type), message, where));
        }
        if (s_previousHandler)
            s_previousHandler(type, context, message);
    }
}

// ---------------------------------------------------------------------------
// Start-up
// ---------------------------------------------------------------------------
bool XDebug::initialize(const QStringList& searchPaths, const QString& fileName)
{
    QMutexLocker lock(&s_initMutex);
    State& s = state();
    if (s.initialized)
        return s.hasForced ? s.forcedEnabled : s.enabled;

    s.fileName = fileName.trimmed().isEmpty() ? defaultFileName() : fileName.trimmed();
    s.searchPaths.clear();
    for (const QString& d : (searchPaths.isEmpty() ? defaultSearchPaths() : searchPaths))
    {
        const QString p = toPosix(d);
        if (!p.isEmpty() && !s.searchPaths.contains(p))
            s.searchPaths.push_back(p);
    }

    s.filePath = findFile(s.searchPaths, s.fileName);
    s.enabled = !s.filePath.isEmpty();
    s.lastError.clear();
    s.options = s.enabled ? parseOptions(s.filePath, s.lastError) : QVariantMap();
    s.initialized = true;
    return s.enabled;
}

bool XDebug::isInitialized()
{
    return state().initialized;
}

QString XDebug::defaultFileName()
{
    QString app = QCoreApplication::applicationName().trimmed().toLower();
    if (app.isEmpty() && QCoreApplication::instance())
        app = QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName().toLower();
    if (app.isEmpty())
        return QStringLiteral("debug.json");
    return app + QStringLiteral("_debug.json");
}

QStringList XDebug::defaultSearchPaths()
{
    QStringList paths;
    auto add = [&paths](const QString& p) {
        const QString n = toPosix(p);
        if (!n.isEmpty() && !paths.contains(n))
            paths.push_back(n);
    };

    // 1. C:/<ApplicationName>  (system drive root + app name, e.g. C:/LMS)
    QString app = QCoreApplication::applicationName().trimmed();
    if (app.isEmpty() && QCoreApplication::instance())
        app = QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName();
    if (!app.isEmpty())
    {
#ifdef Q_OS_WIN
        QString drive = qEnvironmentVariable("SystemDrive", "C:");
        if (!drive.endsWith('/') && !drive.endsWith('\\'))
            drive += '/';
        add(drive + app);
#else
        add("/opt/" + app);
#endif
    }

    // 2. next to the executable
    if (QCoreApplication::instance())
        add(QCoreApplication::applicationDirPath());

    // 3. current working directory
    add(QDir::currentPath());
    return paths;
}

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------
bool XDebug::isEnabled()
{
    ensureInitialized();
    const State& s = state();
    return s.hasForced ? s.forcedEnabled : s.enabled;
}

QString XDebug::filePath()
{
    ensureInitialized();
    return state().filePath;
}

QString XDebug::fileName()
{
    ensureInitialized();
    return state().fileName;
}

QStringList XDebug::searchPaths()
{
    ensureInitialized();
    return state().searchPaths;
}

QString XDebug::lastError()
{
    ensureInitialized();
    return state().lastError;
}

void XDebug::setForcedEnabled(bool enabled)
{
    ensureInitialized();
    state().hasForced = true;
    state().forcedEnabled = enabled;
}

void XDebug::clearForcedEnabled()
{
    ensureInitialized();
    state().hasForced = false;
}

// ---------------------------------------------------------------------------
// Options
// ---------------------------------------------------------------------------
bool XDebug::flag(const QString& name, bool fallback)
{
    if (!isEnabled())
        return false;
    const QVariant* v = findOption(name);
    return v ? isTruthy(*v) : fallback;
}

QString XDebug::value(const QString& key, const QString& fallback)
{
    const QVariant* v = findOption(key);
    return v ? v->toString() : fallback;
}

int XDebug::intValue(const QString& key, int fallback)
{
    const QVariant* v = findOption(key);
    if (!v)
        return fallback;
    bool ok = false;
    const int i = v->toInt(&ok);
    return ok ? i : fallback;
}

double XDebug::doubleValue(const QString& key, double fallback)
{
    const QVariant* v = findOption(key);
    if (!v)
        return fallback;
    bool ok = false;
    const double d = v->toDouble(&ok);
    return ok ? d : fallback;
}

QVariantMap XDebug::options()
{
    ensureInitialized();
    return state().options;
}

bool XDebug::hasOption(const QString& key)
{
    ensureInitialized();
    return state().options.contains(normalizeKey(key));
}

// ---------------------------------------------------------------------------
// Logging
// ---------------------------------------------------------------------------
QString XDebug::logFile()
{
    ensureInitialized();
    const State& s = state();
    if (!s.logFileOverride.isEmpty())
        return s.logFileOverride;
    const QVariant opt = s.options.value("log");
    if (opt.typeId() == QMetaType::QString)
    {
        const QString fromOption = opt.toString().trimmed();
        if (!fromOption.isEmpty())
            return toPosix(QFileInfo(fromOption).isRelative()
                               ? QFileInfo(s.filePath).absolutePath() + "/" + fromOption
                               : fromOption);
    }
    if (!s.filePath.isEmpty())
        return QFileInfo(s.filePath).absolutePath() + "/" + kDefaultLogName;
    return QString();
}

void XDebug::setLogFile(const QString& path)
{
    ensureInitialized();
    state().logFileOverride = toPosix(path);
}

void XDebug::log(const QString& message)
{
    if (!isEnabled())
        return;
    appendToLog(QString("[%1] [LOG] %2")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"), message));
}

void XDebug::installMessageHandler()
{
    if (s_handlerInstalled)
        return;
    s_previousHandler = qInstallMessageHandler(&messageHandler);
    s_handlerInstalled = true;
}

void XDebug::removeMessageHandler()
{
    if (!s_handlerInstalled)
        return;
    qInstallMessageHandler(s_previousHandler);
    s_previousHandler = nullptr;
    s_handlerInstalled = false;
}
