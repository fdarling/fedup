#ifndef FEDUP_MAINWINDOW_H
#define FEDUP_MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QString>

#include "FScintilla.h"
class QsciMacro;

namespace fedup {

class Actions;
class MenuBar;
class ToolBar;
class StatusBar;
class EditPane;
class FindDialog;
class GoToDialog;
class SearchResultsDock;
class TabContext;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = NULL);
    ~MainWindow();
    bool isAlwaysOnTop() const;

signals:
    void sig_AlwaysOnTopChanged(bool onTop);
    void sig_FullscreenChanged(bool onTop);
public slots:
    void open();
    bool open(const QString &filePath);
    bool save();
    bool saveAs();
    bool saveAs(const QString &filePath);
protected slots:
    void _slot_SearchGoTo();
    void _slot_MacroStartRecording();
    void _slot_MacroStopRecording();
    void _slot_MacroPlayback();
    void _slot_MacroRunMultiple();
    void _slot_TabChanged(TabContext *context, TabContext *oldContext);
    void _slot_SetAlwaysOnTop(bool onTop);
    void _slot_SetFullscreen(bool fullscreen);
    void _slot_OpenFileLine(const QString &filePath, int line);
    void _slot_EolModeChanged(FScintilla::EolMode mode);
    void _slot_EditEolModeTriggered(QAction *action);
    void _slot_SaveSettings();
    void _slot_Collapse0();
    void _slot_Collapse1();
    void _slot_Collapse2();
    void _slot_Collapse3();
    void _slot_Collapse4();
    void _slot_Collapse5();
    void _slot_Collapse6();
    void _slot_Collapse7();
    void _slot_Uncollapse0();
    void _slot_Uncollapse1();
    void _slot_Uncollapse2();
    void _slot_Uncollapse3();
    void _slot_Uncollapse4();
    void _slot_Uncollapse5();
    void _slot_Uncollapse6();
    void _slot_Uncollapse7();
protected:
    void _SetupActions();
    void _SetupConnections();
    void _SaveSettings(QSettings &settings) const;

    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

    QSettings _settings;
    QString _currentDirectory;
    Actions *_actions;
    MenuBar *_menubar;
    ToolBar *_toolbar;
    StatusBar *_statusbar;
    EditPane *_editpane;
    FindDialog *_findDialog;
    GoToDialog *_gotoDialog;
    SearchResultsDock *_searchResultsDock;
    QsciMacro * _editorMacro;
};

} // namespace fedup

#endif
