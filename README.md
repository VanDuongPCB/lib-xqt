# lib-xqt

A customized Qt framework that overrides and extends core Qt classes, widgets, and components.

## Layout

One header and one source file per class, named after the class:

    include/XGlobal.h           export macro, common helpers (style classes, repolish)
    include/XPushButton.h       class XPushButton : public QPushButton
    include/XLineEdit.h         class XLineEdit   : public QLineEdit
    include/...                 one file per Qt widget (66 classes)
    include/xQt.h               umbrella header including every widget
    src/XGlobal.cpp
    src/XPushButton.cpp         implementation of XPushButton (empty until customized)
    src/...
    tools/generate_widgets.py   generates the X* .h/.cpp pairs

Include what you use:

    #include "XPushButton.h"
    #include "XTableView.h"

Wrapped classes: buttons (XPushButton, XToolButton, XCheckBox, XRadioButton,
XCommandLinkButton), inputs (XLineEdit, XTextEdit, XPlainTextEdit, XSpinBox,
XDoubleSpinBox, XDateEdit, XTimeEdit, XDateTimeEdit, XComboBox, XFontComboBox,
XKeySequenceEdit, XSlider, XDial, XScrollBar, XCalendarWidget), display (XLabel,
XLCDNumber, XProgressBar, XTextBrowser, XSplashScreen), item views (XListView,
XListWidget, XTableView, XTableWidget, XTreeView, XTreeWidget, XColumnView,
XHeaderView, XUndoView, XGraphicsView), containers (XWidget, XFrame, XGroupBox,
XScrollArea, XStackedWidget, XTabWidget, XTabBar, XToolBox, XSplitter,
XDockWidget, XMdiArea, XMdiSubWindow, XFocusFrame, XRubberBand, XSizeGrip),
windows (XMainWindow, XMenuBar, XMenu, XToolBar, XStatusBar, XDialogButtonBox),
dialogs (XDialog, XMessageBox, XFileDialog, XColorDialog, XFontDialog,
XInputDialog, XProgressDialog, XErrorMessage, XWizard, XWizardPage).

Every `X*` class derives from the Qt class of the same name, inherits all of its
constructors and carries its own `Q_OBJECT`, so style sheets can target it by
name (`XPushButton { ... }`).

## Common API (all X* widgets)

    addStyleClass("primary");          // dynamic property "class" = "primary", style re-applied
    removeStyleClass("primary");
    setStyleClasses({"a", "b"});
    hasStyleClass("a");
    repolish();                        // re-apply the style sheet by hand

    // apply = false: change several classes, then repolish once
    addStyleClass("large", false);
    removeStyleClass("primary", false);
    repolish();

Style sheet selector for a style class:

    XPushButton[class~="primary"] { background: #0a5; }

## XTabBar / XTabWidget: icon and text layout of tabs

`XTabWidget` uses an `XTabBar`, and both expose the tab layout as a
`Qt::ToolButtonStyle`, like QToolButton:

    tabWidget->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // icon above text
    tabWidget->setIconTextSpacing(6);                             // default 4 px

| Value                        | Tabs                                   |
|------------------------------|----------------------------------------|
| Qt::ToolButtonFollowStyle    | default QTabBar look (icon left of text) |
| Qt::ToolButtonIconOnly       | icon only                              |
| Qt::ToolButtonTextOnly       | text only                              |
| Qt::ToolButtonTextBesideIcon | icon left of text, centered as a group |
| Qt::ToolButtonTextUnderIcon  | icon above text                        |

From a style sheet (themes can decide the layout):

    XTabWidget { qproperty-toolButtonStyle: ToolButtonTextUnderIcon; qproperty-iconTextSpacing: 6; }

Padding and spacing:

    tabWidget->setTabPaddingLeft(12);                 // extra space inside every tab (left side);
    tabWidget->setTabPadding(QMargins(12, 4, 12, 4)); // also top/right/bottom; the tab grows and the
                                                      // content moves by the padding, like CSS padding
    tabWidget->setTabBarLeftMargin(16);               // gap between the widget's left edge and the first tab

    XTabWidget { qproperty-tabPaddingLeft: 12; qproperty-tabBarLeftMargin: 16; }

Tab sizes follow the layout (TextUnderIcon tabs are taller and narrower).
Text color and font still come from the style sheet (`QTabBar::tab { color: ... }`).
Vertical tab bars (West/East) keep the default painting. `XTabWidget::xTabBar()`
returns the typed tab bar. These two classes are hand-written, not generated.

## XStyleManager: themes from a directory

`XStyleManager` (include/XStyleManager.h) loads a theme folder and turns it
into one Qt style sheet.

    <themeRoot>/<ThemeName>/
        css/        *.css or *.qss, concatenated in case-insensitive name order
        data/       variables.json: flat {"key": "value"} map
        images/     referenced from css through @imageDir

Variables are written as `@name` in the css and replaced at load time.
Built-in: `@themeName`, `@themeDir`, `@imageDir`. Unknown tokens are kept.

    /* css/button.css */
    XPushButton { background: @primary; border-image: url(@imageDir/btn.png); }

    /* data/variables.json */
    { "primary": "#0a84ff", "radius": "4px" }

Usage:

    auto* styles = XStyleManager::instance();
    styles->setThemeRoot(qApp->applicationDirPath() + "/Themes");
    styles->setWatchEnabled(true);                   // hot reload while editing
    styles->loadTheme("Default");                    // applied to qApp (autoApply)

    styles->availableThemes();                       // folders that contain css/
    styles->styleSheet({"messagebox"});              // one file by base name
    styles->applyTo(&dialog, {"dialog", "button"});
    styles->color("primary");                        // QColor from a variable
    styles->setVariable("primary", "#ff0000");       // overrides, survives reload
    styles->reload();

Signals: `themeChanged(name)`, `styleSheetChanged()`, `errorOccurred(message)`.
Use `lastError()` after a failed `loadTheme()`.

The theme root may be a Qt resource path (`setThemeRoot(":/themes")`) when the
theme folders are compiled into the application with a .qrc file; everything
works the same except hot reload, which needs a real directory.

## XDebug: debug mode switched by a JSON file

`XDebug` (include/XDebug.h) enables debug mode when a developer-created file
`<applicationName>_debug.json` (lower case, e.g. `lms_debug.json` for LMS.exe)
exists in one of these places, checked in order:

    1. C:/<ApplicationName>/      e.g. C:/LMS/lms_debug.json
    2. next to the executable
    3. the current working directory

Create the file to enable, delete it to disable; no rebuild. `{}` or an empty
file is enough; flags and values are optional:

    {
        "verbose": true,
        "plc": "off",
        "laser": "simulate",
        "retries": 5,
        "log": "lms.log",                  // log file; default debug.log next to the json
        "hardware": { "barcode": false }   // nested objects -> "hardware.barcode"
    }

The file is read once at start-up and never re-read; the data is immutable
afterwards, so every function is safe to call from any thread.

Usage (main.cpp):

    QApplication app(argc, argv);
    XDebug::initialize();                          // optional, otherwise the first call does it
    XDebug::installMessageHandler();               // qDebug/qWarning/qCritical also go to the log while enabled

Anywhere else:

    if (XDebug::isEnabled()) ...
    if (XDebug::flag("verbose")) ...               // true, non-zero number, "1"/"true"/"on"/"yes"
    if (XDebug::value("plc") == "off") ...
    if (!XDebug::flag("hardware.barcode", true)) ...
    int n = XDebug::intValue("retries", 3);
    XDEBUG() << "printed only in debug mode";
    XDebug::log("marking started");               // "[time] [LOG] marking started"

Keys are case-insensitive. A file with invalid JSON still enables debug mode;
the parse error is available from `lastError()`.

Other: `initialize(searchPaths, fileName)` to use other locations or another
file name (must be the first call), `setForcedEnabled(bool)` (e.g. from a
`--debug` argument), `setLogFile(path)`.

## Build

Standalone:

    cmake -S . -B build -DCMAKE_PREFIX_PATH=C:/Qt/6.x/msvc2022_64
    cmake --build build --config RelWithDebInfo

As a submodule, from the parent project:

    add_subdirectory(xQt)
    target_link_libraries(app PRIVATE xQt::xQt)

Options: `-DXQT_BUILD_SHARED=ON` builds a DLL instead of a static library.

## Regenerating the wrappers

Edit the class list in `tools/generate_widgets.py`, then:

    python tools/generate_widgets.py

The script rewrites the X*.h/.cpp pairs and xQt.h, and deletes files of classes
that were removed from the list. Hand-written code belongs in the .cpp files
only if you also remove the class from the generator list; otherwise it will be
overwritten on the next run.
