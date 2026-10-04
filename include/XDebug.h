#pragma once

#include <QString>
#include <QStringList>
#include <QVariantMap>

#include "XGlobal.h"

/*
    XDebug: runtime debug switch controlled by a JSON file written by the developer.
    The file is read ONCE at application start-up; after that nothing changes.

    Debug mode is enabled when the debug file exists in one of the search paths.
    Default search order:
        1. C:/<ApplicationName>/            e.g. C:/LMS/lms_debug.json
        2. the application directory        next to the .exe
        3. the current working directory

    File name: "<applicationName>_debug.json" in lower case, e.g. "lms_debug.json"
    for LMS.exe.

    Content: a flat JSON object of flags and values. {} or an empty file is fine.

        {
            "verbose": true,
            "plc": "off",
            "laser": "simulate",
            "retries": 5,
            "log": "lms.log",                  // optional log file; default debug.log next to the json
            "hardware": { "barcode": false }   // nested objects become "hardware.barcode"
        }

    Usage (main.cpp):

        QApplication app(argc, argv);
        XDebug::initialize();                  // optional: explicit load at start-up
        XDebug::installMessageHandler();       // qDebug/qWarning -> log while enabled

    Anywhere else:

        if (XDebug::isEnabled()) ...
        if (XDebug::flag("verbose")) ...
        if (XDebug::value("plc") == "off") ...
        if (!XDebug::flag("hardware.barcode", true)) ...
        XDEBUG() << "only printed in debug mode";
        XDebug::log("marking started");

    Every accessor initializes with the defaults if initialize() was not called.
    Since the data is immutable after loading, all functions are safe to call
    from any thread.
*/
class XQT_EXPORT XDebug
{
public:
    static constexpr const char* kDefaultLogName = "debug.log";

    XDebug() = delete;

    // --- start-up ----------------------------------------------------------
    // Loads the file once. Later calls are ignored (returns the existing state).
    // Empty arguments mean defaultSearchPaths() / defaultFileName().
    static bool initialize(const QStringList& searchPaths = {}, const QString& fileName = {});
    static bool isInitialized();

    static QString defaultFileName();                  // "<applicationName>_debug.json", lower case
    static QStringList defaultSearchPaths();           // C:/<AppName>, application dir, current dir

    // --- state -----------------------------------------------------------
    static bool isEnabled();
    static QString filePath();                         // debug file that was found, or empty
    static QString fileName();
    static QStringList searchPaths();
    static QString lastError();                        // e.g. JSON parse error (file still enables debug)

    // Manual override, e.g. from a "--debug" command line argument.
    static void setForcedEnabled(bool enabled);
    static void clearForcedEnabled();

    // --- options from the json file ---------------------------------------
    // Keys are case-insensitive; nested objects are flattened with '.'.
    // All of them return the fallback (or false) while debug mode is disabled.
    static bool flag(const QString& name, bool fallback = false);   // true, non-zero number, "1/true/on/yes"
    static QString value(const QString& key, const QString& fallback = QString());
    static int intValue(const QString& key, int fallback = 0);
    static double doubleValue(const QString& key, double fallback = 0.0);
    static QVariantMap options();
    static bool hasOption(const QString& key);

    // --- logging ---------------------------------------------------------
    static QString logFile();                          // option "log" or debug.log next to the json
    static void setLogFile(const QString& path);       // overrides the option
    static void log(const QString& message);           // appends a timestamped line when enabled
    static void installMessageHandler();               // qDebug/qInfo/qWarning/qCritical -> log while enabled
    static void removeMessageHandler();
};

// Streams like qDebug(), but only when debug mode is enabled.
#define XDEBUG() if (!XDebug::isEnabled()) {} else qDebug()
