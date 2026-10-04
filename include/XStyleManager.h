#pragma once

#include <QColor>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

#include "XGlobal.h"

class QFileSystemWatcher;
class QTimer;
class QWidget;

/*
    XStyleManager loads a theme from a directory and turns it into one Qt style
    sheet.

    Theme folder layout (everything optional except css/):

        <themeRoot>/<ThemeName>/
            css/        *.css or *.qss, concatenated in case-insensitive name order
            data/       variables.json: flat {"key": "value"} map (colors, sizes, fonts...)
            images/     referenced from css through @imageDir

    Variables are written as @name inside the css files and replaced when the
    theme is loaded. Built-in variables: @themeName, @themeDir, @imageDir.
    Unknown @tokens are left untouched.

    Typical use:

        auto* styles = XStyleManager::instance();
        styles->setThemeRoot(qApp->applicationDirPath() + "/Themes");
        styles->setWatchEnabled(true);          // hot reload while editing css
        styles->loadTheme("Default");           // applies to qApp (autoApply = true)

        msgBox.setStyleSheet(styles->styleSheet({"messagebox"}));
        QColor accent = styles->color("accent");
*/
class XQT_EXPORT XStyleManager : public QObject
{
    Q_OBJECT
public:
    explicit XStyleManager(QObject* parent = nullptr);
    ~XStyleManager() override;

    // Application-wide instance (created on first call, owned by qApp if present).
    static XStyleManager* instance();

    // Theme root and discovery
    void setThemeRoot(const QString& dir);
    QString themeRoot() const;
    QStringList availableThemes() const;

    // Loading
    bool loadTheme(const QString& name);
    bool reload();
    QString currentTheme() const;
    QString themeDir() const;
    QString imageDir() const;
    bool isLoaded() const;

    // Variables (data/variables.json + setVariable). setVariable survives reload.
    void setVariable(const QString& key, const QString& value);
    void removeVariable(const QString& key);
    QString variable(const QString& key, const QString& fallback = QString()) const;
    QColor color(const QString& key, const QColor& fallback = QColor()) const;
    QVariantMap variables() const;

    // Style sheets
    QString styleSheet() const;                          // every file, in order
    QString styleSheet(const QStringList& names) const;  // selected files by base name ("button")
    QStringList styleSheetNames() const;                 // base names in load order

    // Applying
    void setAutoApply(bool enabled);                     // default: true
    bool autoApply() const;
    void applyToApplication() const;                     // qApp->setStyleSheet(styleSheet())
    void applyTo(QWidget* widget, const QStringList& names = {}) const;

    // Hot reload
    void setWatchEnabled(bool enabled);                  // default: false
    bool isWatchEnabled() const;

    QString lastError() const;

signals:
    void themeChanged(const QString& name);              // after a successful loadTheme/reload
    void styleSheetChanged();                            // style sheet text changed
    void errorOccurred(const QString& message);

private:
    bool readTheme(const QString& name);
    bool readVariables(const QString& dir, QVariantMap& out);
    bool readStyleSheets(const QString& dir, QStringList& names, QStringList& contents);
    QString substitute(const QString& css) const;
    void updateWatcher();
    void onWatchedPathChanged(const QString& path);
    void setError(const QString& message);

private:
    QString m_themeRoot;
    QString m_themeName;
    QString m_themeDir;
    QVariantMap m_fileVariables;      // from data/variables.json
    QVariantMap m_userVariables;      // from setVariable(), override file variables
    QStringList m_sheetNames;
    QStringList m_sheetContents;      // substituted, same order as m_sheetNames
    QString m_combined;
    QString m_lastError;
    bool m_autoApply = true;
    bool m_watchEnabled = false;
    QFileSystemWatcher* m_watcher = nullptr;
    QTimer* m_reloadTimer = nullptr;
};
