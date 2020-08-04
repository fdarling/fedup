#include "MainWindow.h"
#include "Actions.h"
#include "MenuBar.h"
#include "ToolBar.h"
#include "SearchResultsDock.h"
#include "StatusBar.h"
#include "EditPane.h"
#include "EditPaneTabs.h"
#include "FScintilla.h"
#include "TabContext.h"
#include "FindDialog.h"
#include "GoToDialog.h"
#include "ExitSaveDialog.h"
#include "RecentFilesList.h"
#include "FileFilters.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QCloseEvent>
// #include <QDebug>

#include <limits>

#include <Qsci/qscimacro.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace fedup {

extern const QString FILE_TYPES_STRING; // defined at the bottom of this file

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _settings(QSettings::UserScope, "forestdarling.com", "fedup3"),
    _actions(new Actions(this)),
    _menubar(new MenuBar(_actions, _settings)),
    _toolbar(new ToolBar(_actions)),
    _statusbar(new StatusBar),
    _editpane(new EditPane),
    _findDialog(new FindDialog(_editpane->editor(), this)),
    _gotoDialog(new GoToDialog(this)),
    _searchResultsDock(new SearchResultsDock),
    _editorMacro(NULL)
{
    _toolbar->setObjectName("toolbar");
    _searchResultsDock->setObjectName("search_results");
    setMenuBar(_menubar);
    addToolBar(_toolbar);
    setStatusBar(_statusbar);
    setCentralWidget(_editpane);
    addDockWidget(Qt::BottomDockWidgetArea, _searchResultsDock);
    setContextMenuPolicy(Qt::NoContextMenu); // to prevent the default toolbar/dock widget show/hide context menu

    _SetupActions();
    _SetupConnections();

    if (!restoreGeometry(_settings.value("window_geometry").toByteArray()))
        setGeometry(QRect(50, 50, 800, 600));
    if (!restoreState(_settings.value("window_state").toByteArray()))
        _searchResultsDock->hide();
    _slot_SetAlwaysOnTop(_settings.value("window_always_on_top", QVariant(false)).toBool());
    _currentDirectory = _settings.value("working_directory").toString();
    _findDialog->setCurrentDirectory(_currentDirectory);

    // reopen the previously open files
    _editpane->loadSession(_settings);

    {
        QTimer * const timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::_slot_SaveSettings);
        timer->start(30000); // TODO make this configurable, currently auto-save the program state every 30 seconds
    }
}

MainWindow::~MainWindow()
{
    // TODO save the macro between program relaunches?
    if (_editorMacro)
        delete _editorMacro;
}

void MainWindow::_SetupActions()
{
    FScintilla * const e = _editpane->editor();
    EditPaneTabs * const tabs = _editpane->tabs();

    // make the actions know when they are available
    _actions->fileSave->setEnabled(false);
    _actions->fileSaveAs->setEnabled(false);
    _actions->fileClose->setEnabled(false);
    _actions->fileCloseAll->setEnabled(false);
    connect(e,    &FScintilla::modificationChanged,   _actions->fileSave,     &QAction::setEnabled);
    connect(tabs, &EditPaneTabs::containsTabsChanged, _actions->fileSaveAs,   &QAction::setEnabled);
    connect(tabs, &EditPaneTabs::containsTabsChanged, _actions->fileClose,    &QAction::setEnabled);
    connect(tabs, &EditPaneTabs::containsTabsChanged, _actions->fileCloseAll, &QAction::setEnabled);

    connect(_actions->fileNew, &QAction::triggered, _editpane, &EditPane::openNew);
    connect(_actions->fileOpen, &QAction::triggered, this, qOverload<>(&MainWindow::open));
    connect(_actions->fileReload, &QAction::triggered, _editpane, &EditPane::reload);
    connect(_actions->fileSave, &QAction::triggered, this, &MainWindow::save);
    connect(_actions->fileSaveAs, &QAction::triggered, this, qOverload<>(&MainWindow::saveAs));
    connect(_actions->fileClose, &QAction::triggered, _editpane, &EditPane::closeTab);
    connect(_actions->fileCloseAll, &QAction::triggered, _editpane, &EditPane::closeAll);
    connect(_actions->fileExit, &QAction::triggered, this, &MainWindow::close);

    // make the actions know when they are available
    _actions->editUndo->setEnabled(false);
    _actions->editRedo->setEnabled(false);
    _actions->editCut->setEnabled(false);
    _actions->editCopy->setEnabled(false);
    _actions->editDelete->setEnabled(false);
    connect(e, &FScintilla::undoAvailable, _actions->editUndo, &QAction::setEnabled);
    connect(e, &FScintilla::redoAvailable, _actions->editRedo, &QAction::setEnabled);
    connect(e, &FScintilla::copyAvailable, _actions->editCut, &QAction::setEnabled);
    connect(e, &FScintilla::copyAvailable, _actions->editCopy, &QAction::setEnabled);
    connect(e, &FScintilla::copyAvailable, _actions->editDelete, &QAction::setEnabled);

    connect(_actions->editUndo, &QAction::triggered, e, &FScintilla::undo);
    connect(_actions->editRedo, &QAction::triggered, e, &FScintilla::redo);
    connect(_actions->editCut, &QAction::triggered, e, &FScintilla::cut);
    connect(_actions->editCopy, &QAction::triggered, e, &FScintilla::copy);
    connect(_actions->editPaste, &QAction::triggered, e, &FScintilla::paste);
    connect(_actions->editDelete, &QAction::triggered, e, &FScintilla::removeSelectedText);
    connect(_actions->editSelectAll, &QAction::triggered, e, &FScintilla::selectAll);
    connect(_actions->editDuplicateLines, &QAction::triggered, e, &FScintilla::duplicateLines);
    connect(_actions->editDeleteLine, &QAction::triggered, e, &FScintilla::deleteLine);
    connect(_actions->editUndeleteLines, &QAction::triggered, e, &FScintilla::undeleteLines);
    connect(_actions->editMoveLineUp, &QAction::triggered, e, &FScintilla::moveLineUp);
    connect(_actions->editMoveLineDown, &QAction::triggered, e, &FScintilla::moveLineDown);
    connect(_actions->editToggleBlockComment, &QAction::triggered, e, &FScintilla::toggleCommented);
    connect(_actions->editTrimTrailingSpaces, &QAction::triggered, e, &FScintilla::trimTrailingWhitespace);
    connect(_actions->editSimplifyWhitespace, &QAction::triggered, e, &FScintilla::simplifyWhitespace);
    _slot_EolModeChanged(e->eolMode()); // set the initial value
    connect(_actions->editEolGroup, &QActionGroup::triggered, this, &MainWindow::_slot_EditEolModeTriggered);

    connect(_actions->searchFind, &QAction::triggered, _findDialog, &FindDialog::showFind);
    connect(_actions->searchFindInFiles, &QAction::triggered, _findDialog, &FindDialog::showFindInFiles);
    connect(_actions->searchFindNext, &QAction::triggered, _findDialog, &FindDialog::findNext);
    connect(_actions->searchFindPrev, &QAction::triggered, _findDialog, &FindDialog::findPrev);
    connect(_actions->searchReplace, &QAction::triggered, _findDialog, &FindDialog::showReplace);
    connect(_actions->searchGoTo, &QAction::triggered, this, &MainWindow::_slot_SearchGoTo);
    connect(_actions->searchGoToMatchingBrace, &QAction::triggered, e, &FScintilla::moveToMatchingBrace); // TODO disable this when there isn't a matching brace to go to, I need to have FScintilla emit a signal...
    connect(_actions->searchBookmarkToggle, &QAction::triggered, e, qOverload<>(&FScintilla::toggleBookmark));
    connect(_actions->searchBookmarkNext, &QAction::triggered, e, &FScintilla::findNextBookmark);
    connect(_actions->searchBookmarkPrev, &QAction::triggered, e, &FScintilla::findPrevBookmark);
    connect(_actions->searchBookmarkClearAll, &QAction::triggered, e, &FScintilla::clearBookmarks);

    // make the actions know when they are available
    _actions->viewSearchResults->setChecked(_searchResultsDock->isVisible());
    // _actions->viewFullscreen->setChecked(isFullScreen()); // TODO figure out why this doesn't fully work
    // _actions->viewFullscreen->setChecked(windowState().testFlag(Qt::WindowFullScreen)); // TODO figure out if this is better than the above method, NOTE: this doesn't seem to work, I have to use showEvent to track it instead...
    _actions->viewAlwaysOnTop->setChecked(isAlwaysOnTop());
    _actions->viewSymbolWhitespace->setChecked(e->whitespaceVisibility() != QsciScintilla::WsInvisible);
    _actions->viewSymbolNewlines->setChecked(e->eolVisibility());
    _actions->viewIndentationGuide->setChecked(e->indentationGuides());
    connect(_searchResultsDock, &SearchResultsDock::visibilityChanged, _actions->viewSearchResults, &QAction::setChecked);
    connect(this, &MainWindow::sig_AlwaysOnTopChanged, _actions->viewAlwaysOnTop, &QAction::setChecked);
    connect(this, &MainWindow::sig_FullscreenChanged, _actions->viewFullscreen, &QAction::setChecked);

    connect(_actions->viewAlwaysOnTop, &QAction::toggled, this, &MainWindow::_slot_SetAlwaysOnTop);
    connect(_actions->viewFullscreen, &QAction::toggled, this, &MainWindow::_slot_SetFullscreen);
    connect(_actions->viewSymbolWhitespace, &QAction::toggled, e, &FScintilla::setWhitespaceVisible);
    connect(_actions->viewIndentationGuide, &QAction::toggled, e, &FScintilla::setIndentationGuides);
    connect(_actions->viewSymbolNewlines, &QAction::toggled, e, &FScintilla::setEolVisibility);
    connect(_actions->viewFoldAll, &QAction::triggered, e, &FScintilla::foldAll);
    connect(_actions->viewUnfoldAll, &QAction::triggered, e, &FScintilla::unfoldAll);
    connect(_actions->viewCollapse1, &QAction::triggered, this, &MainWindow::_slot_Collapse0);
    connect(_actions->viewCollapse2, &QAction::triggered, this, &MainWindow::_slot_Collapse1);
    connect(_actions->viewCollapse3, &QAction::triggered, this, &MainWindow::_slot_Collapse2);
    connect(_actions->viewCollapse4, &QAction::triggered, this, &MainWindow::_slot_Collapse3);
    connect(_actions->viewCollapse5, &QAction::triggered, this, &MainWindow::_slot_Collapse4);
    connect(_actions->viewCollapse6, &QAction::triggered, this, &MainWindow::_slot_Collapse5);
    connect(_actions->viewCollapse7, &QAction::triggered, this, &MainWindow::_slot_Collapse6);
    connect(_actions->viewCollapse8, &QAction::triggered, this, &MainWindow::_slot_Collapse7);
    connect(_actions->viewUncollapse1, &QAction::triggered, this, &MainWindow::_slot_Uncollapse0);
    connect(_actions->viewUncollapse2, &QAction::triggered, this, &MainWindow::_slot_Uncollapse1);
    connect(_actions->viewUncollapse3, &QAction::triggered, this, &MainWindow::_slot_Uncollapse2);
    connect(_actions->viewUncollapse4, &QAction::triggered, this, &MainWindow::_slot_Uncollapse3);
    connect(_actions->viewUncollapse5, &QAction::triggered, this, &MainWindow::_slot_Uncollapse4);
    connect(_actions->viewUncollapse6, &QAction::triggered, this, &MainWindow::_slot_Uncollapse5);
    connect(_actions->viewUncollapse7, &QAction::triggered, this, &MainWindow::_slot_Uncollapse6);
    connect(_actions->viewUncollapse8, &QAction::triggered, this, &MainWindow::_slot_Uncollapse7);
    connect(_actions->viewSearchResults, &QAction::toggled, _searchResultsDock, &SearchResultsDock::setVisible);

    // make the actions know when they are available
    _actions->macroStartRecording->setEnabled(true);
    _actions->macroStopRecording->setEnabled(false);
    _actions->macroPlayback->setEnabled(false);
    _actions->macroRunMultiple->setEnabled(false);
    connect(_actions->macroStartRecording, &QAction::triggered, this, &MainWindow::_slot_MacroStartRecording);
    connect(_actions->macroStopRecording, &QAction::triggered, this, &MainWindow::_slot_MacroStopRecording);
    connect(_actions->macroPlayback, &QAction::triggered, this, &MainWindow::_slot_MacroPlayback);
    connect(_actions->macroRunMultiple, &QAction::triggered, this, &MainWindow::_slot_MacroRunMultiple);
}

void MainWindow::_SetupConnections()
{
    FScintilla * const e = _editpane->editor();
    EditPaneTabs * const tabs = _editpane->tabs();

    connect(e, &FScintilla::cursorPositionChanged, _statusbar, &StatusBar::slot_SetCursorPosition);
    connect(e, &FScintilla::selectionLengthChanged, _statusbar, &StatusBar::slot_SetSelectionLength);
    connect(e, &FScintilla::lineCountChanged, _statusbar, &StatusBar::slot_SetLineCount);
    connect(e, &FScintilla::lengthChanged, _statusbar, &StatusBar::slot_SetLength);
    connect(e, &FScintilla::lineChanged, _gotoDialog, &GoToDialog::slot_SetCurrentLine);
    connect(e, &FScintilla::positionChanged, _gotoDialog, &GoToDialog::slot_SetCurrentOffset);
    connect(e, &FScintilla::lineCountChanged, _gotoDialog, &GoToDialog::slot_SetMaxLine);
    connect(e, &FScintilla::lengthChanged, _gotoDialog, &GoToDialog::slot_SetMaxOffset); // TODO use slot_SetLength instead?
    connect(e, &FScintilla::eolModeChanged, this, &MainWindow::_slot_EolModeChanged);
    connect(_gotoDialog, &GoToDialog::goToLine, e, &FScintilla::goToLine);
    connect(_gotoDialog, &GoToDialog::goToOffset, e, &FScintilla::goToOffset);

    connect(_menubar->recentFilesList(), &RecentFilesList::recentFileClicked, this, qOverload<const QString &>(&MainWindow::open));

    connect(tabs, &EditPaneTabs::tabChanged, this, &MainWindow::_slot_TabChanged);

    connect(_findDialog, &FindDialog::sig_SearchStarted, _searchResultsDock, &SearchResultsDock::startSearch);
    connect(_findDialog, &FindDialog::sig_ResultFound, _searchResultsDock, &SearchResultsDock::addResult);
    connect(_findDialog, &FindDialog::sig_SearchEnded, _searchResultsDock, &SearchResultsDock::endSearch);

    connect(_searchResultsDock, &SearchResultsDock::requestOpenFileLine, this, &MainWindow::_slot_OpenFileLine);
}

void MainWindow::open()
{
    const QStringList fileList = QFileDialog::getOpenFileNames(this, "Open...", _currentDirectory, FILE_FILTERS);
    for (QStringList::const_iterator it = fileList.begin(); it != fileList.end(); ++it)
        open(*it);
}

bool MainWindow::open(const QString &filePath)
{
    QFileInfo info(filePath);
    _currentDirectory = info.absolutePath();
    const QString absoluteFilePath = info.absoluteFilePath();
    const OpenResult openResult = _editpane->open(filePath);
    switch (openResult)
    {
        case OpenSucceeded:
        _menubar->recentFilesList()->slot_FileOpened(absoluteFilePath); // TODO use signals to communicate that a file opened?
        break;

        case OpenDoesntExist:
        {
            QMessageBox::StandardButton result = QMessageBox::question(this, "Create new file?", "\"" + absoluteFilePath + "\" doesn't exist. Create it?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (result == QMessageBox::Yes)
            {
                _editpane->openNew(); // TODO have it named already
                saveAs(absoluteFilePath);
                // HACK TODO make some way of directly attempting to open the file with a name and save it,
                // while checking for errors and outputting useful warnings. Currently it temporarily opens
                // a new tab, saves it, and possibly closes it if the save *apparently* failed because the
                // context wasn't updated. Also, as a side effect the "new XXX" number is incremented needlessly,
                // and the "new XXX" tab is momentarily shown before an error message
                if (_editpane->tabs()->tabContext(_editpane->tabs()->currentIndex())->filePath.size() == 0)
                    _editpane->closeTab();
            }
        }
        return false;

        case OpenAccessDenied:
        QMessageBox::warning(this, "Access denied", "Can not open file \"" + absoluteFilePath + "\"");
        return false;

        case OpenReadError:
        QMessageBox::warning(this, "Read error", "Can not read file \"" + absoluteFilePath + "\"");
        return false;

        case OpenAlreadyOpen:
        break;
    }
    return true;
}

bool MainWindow::save()
{
    EditPaneTabs * const tabs = _editpane->tabs();
    if (tabs->count() == 0)
        return false;
    TabContext * const context = tabs->tabContext(tabs->currentIndex());
    if (context->filePath.size() == 0)
        return saveAs();
    else
        return saveAs(context->filePath);
}

bool MainWindow::saveAs()
{
    const QString filename = QFileDialog::getSaveFileName(this, "Save As...", _currentDirectory, FILE_FILTERS);
    if (filename.size() == 0)
        return false;

    return saveAs(filename);
}

bool MainWindow::saveAs(const QString &filePath)
{
    QFileInfo info(filePath);
    const QString absoluteFilePath = info.absoluteFilePath();
    const SaveResult saveResult = _editpane->saveAs(filePath);
    switch (saveResult)
    {
        case SaveNothingToSave:
        // NOTE: this shouldn't happen normally unless saveAs is programatically called rather than from an action
        break;

        case SaveSucceeded:
        _currentDirectory = info.absolutePath();
        return true;

        case SaveAccessDenied:
        QMessageBox::warning(this, "Access denied", "Error opening \"" + absoluteFilePath + "\" for writing");
        break;

        case SaveDirectoryDoesntExist:
        QMessageBox::warning(this, "Directory doesn't exist", "Error opening \"" + absoluteFilePath + "\" for writing");
        break;

        case SaveWriteError:
        QMessageBox::warning(this, "Write error", "Error writing \"" + absoluteFilePath + "\"");
        break;
    }

    return false;
}

void MainWindow::_slot_SearchGoTo()
{
    _gotoDialog->show();
    _gotoDialog->raise();
    _gotoDialog->activateWindow();
}

void MainWindow::_slot_MacroStartRecording()
{
    _actions->macroStartRecording->setEnabled(false);
    _actions->macroStopRecording->setEnabled(true);
    FScintilla * const editor = _editpane->editor();
    if (!_editorMacro || _editorMacro->parent() != editor)
    {
        if (_editorMacro)
            delete _editorMacro;
        _editorMacro = new QsciMacro(editor);
        // connect(_editorMacro, &QsciMacro::destroyed, this, &MainWindow::slot_MacroDestroyed);
    }
    _editorMacro->startRecording();
}

void MainWindow::_slot_MacroStopRecording()
{
    _editorMacro->endRecording();
    _actions->macroStartRecording->setEnabled(true);
    _actions->macroStopRecording->setEnabled(false);
    _actions->macroPlayback->setEnabled(true);
    _actions->macroRunMultiple->setEnabled(true);
}

void MainWindow::_slot_MacroPlayback()
{
    FScintilla * const editor = _editpane->editor();
    if (_editorMacro->parent() != editor)
    {
        QsciMacro * const oldMacro = _editorMacro;
        const QString saved = oldMacro->save();
        _editorMacro = new QsciMacro(saved, editor);
        // connect(_editorMacro, &QsciMacro::destroyed, this, &MainWindow::slot_MacroDestroyed);
        delete oldMacro;
    }
    _editorMacro->play();
}

void MainWindow::_slot_MacroRunMultiple()
{
    // TODO save the numberOfTimes between dialog showings, and have a better dialog that supports "Run until the end of the file"
    bool ok = false;
    int numberOfTimes = QInputDialog::getInt(this, "Run a Macro Multiple Times", "Times:", 1, 1, std::numeric_limits<int>::max(), 1, &ok);
    if (ok)
    {
        FScintilla * const editor = _editpane->editor();
        if (_editorMacro->parent() != editor)
        {
            QsciMacro * const oldMacro = _editorMacro;
            const QString saved = oldMacro->save();
            _editorMacro = new QsciMacro(saved, editor);
            // connect(_editorMacro, &QsciMacro::destroyed, this, &MainWindow::slot_MacroDestroyed);
            delete oldMacro;
        }
        for (; numberOfTimes > 0; numberOfTimes--)
            _editorMacro->play();
    }
}

/*void MainWindow::_slot_MacroDestroyed()
{
    if (!editorMacro)
    {
        macroStartAction->setEnabled(tabs->currentEditor() != NULL);
        macroStopAction->setEnabled(false);
        macroPlayAction->setEnabled(false);
        macroPlayMultiAction->setEnabled(false);
    }
}*/

void MainWindow::_slot_TabChanged(TabContext *context, TabContext *oldContext)
{
    Q_UNUSED(oldContext);
    // TODO deal with unsaved (new) files
    // TODO have the editpane emit a signal when a file's path is changed
    // qDebug() << "Context changed from " << (oldContext ? oldContext->filePath : QString("<NULL>")) << " to " << (context ? context->filePath : QString("<NULL>"));
    // FScintilla * const e = _editpane->editor();
    // qDebug() << "modified?" << e->isModified();
    if (context)
    {
        setWindowTitle(context->filePath + " - fedup");
        if (!context->filePath.isEmpty())
            _currentDirectory = QFileInfo(context->filePath).absolutePath();
        _findDialog->setCurrentDirectory(_currentDirectory); // TODO use a signal to communicate instead? perhaps it isn't necessary...
    }
    else
        setWindowTitle("fedup");
}

static const Qt::WindowFlags onTopFlags = (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);

void MainWindow::_slot_SetAlwaysOnTop(bool onTop)
{
    // TODO check if we are already set the way we want and do not re-emit a signal if so
#ifdef Q_OS_WIN
    if (onTop)
        SetWindowPos(winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    else
        SetWindowPos(winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#else
    Qt::WindowFlags flags = windowFlags();
    if (onTop)
        flags |= onTopFlags;
    else
        flags &= ~onTopFlags;
    setWindowFlags(flags);
    show();
#endif
    emit sig_AlwaysOnTopChanged(onTop);
}

bool MainWindow::isAlwaysOnTop() const
{
#ifdef Q_OS_WIN
    return ((GetWindowLong(winId(), GWL_EXSTYLE) & WS_EX_TOPMOST) == WS_EX_TOPMOST);
#else
    return (windowFlags() & onTopFlags) == onTopFlags;
#endif
}

void MainWindow::_slot_SetFullscreen(bool fullscreen)
{
    if (windowState().testFlag(Qt::WindowFullScreen) == fullscreen)
        return;
    /*if (fullscreen)
        showFullScreen();
    else
        showNormal();*/
    // TODO find out which method is better, perhaps they are equivalent?
    // NOTE: I figured out the above method doesn't account for showMaximized(), this the below method is better
    if (fullscreen)
        setWindowState(windowState() | Qt::WindowFullScreen);
    else
        setWindowState(windowState() & ~Qt::WindowFullScreen);
    emit sig_FullscreenChanged(fullscreen);
}

void MainWindow::_slot_OpenFileLine(const QString &filePath, int line)
{
    if (open(filePath))
        _editpane->editor()->goToLine(line);
}

void MainWindow::_slot_EolModeChanged(FScintilla::EolMode mode)
{
    const QList<QAction*> l = _actions->editEolGroup->actions();
    for (QList<QAction*>::const_iterator it = l.begin(); it != l.end(); ++it)
    {
        if (mode == static_cast<FScintilla::EolMode>((*it)->data().toInt()))
        {
            (*it)->setChecked(true);
            break;
        }
    }
}

void MainWindow::_slot_EditEolModeTriggered(QAction *action)
{
    _editpane->editor()->setEolMode(static_cast<FScintilla::EolMode>(action->data().toInt()));
}

void MainWindow::_slot_SaveSettings()
{
    _SaveSettings(_settings);
}

void MainWindow::_slot_Collapse0()
{
    _editpane->editor()->collapse(0, false);
}

void MainWindow::_slot_Collapse1()
{
    _editpane->editor()->collapse(1, false);
}

void MainWindow::_slot_Collapse2()
{
    _editpane->editor()->collapse(2, false);
}

void MainWindow::_slot_Collapse3()
{
    _editpane->editor()->collapse(3, false);
}

void MainWindow::_slot_Collapse4()
{
    _editpane->editor()->collapse(4, false);
}

void MainWindow::_slot_Collapse5()
{
    _editpane->editor()->collapse(5, false);
}

void MainWindow::_slot_Collapse6()
{
    _editpane->editor()->collapse(6, false);
}

void MainWindow::_slot_Collapse7()
{
    _editpane->editor()->collapse(7, false);
}

void MainWindow::_slot_Uncollapse0()
{
    _editpane->editor()->collapse(0, true);
}

void MainWindow::_slot_Uncollapse1()
{
    _editpane->editor()->collapse(1, true);
}

void MainWindow::_slot_Uncollapse2()
{
    _editpane->editor()->collapse(2, true);
}

void MainWindow::_slot_Uncollapse3()
{
    _editpane->editor()->collapse(3, true);
}

void MainWindow::_slot_Uncollapse4()
{
    _editpane->editor()->collapse(4, true);
}

void MainWindow::_slot_Uncollapse5()
{
    _editpane->editor()->collapse(5, true);
}

void MainWindow::_slot_Uncollapse6()
{
    _editpane->editor()->collapse(6, true);
}

void MainWindow::_slot_Uncollapse7()
{
    _editpane->editor()->collapse(7, true);
}

void MainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    _actions->viewFullscreen->setChecked(isFullScreen());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ExitSaveDialog exitSaveDialog(this);
    if (exitSaveDialog.shouldClose(this, _editpane))
    {
        _slot_SaveSettings();
        event->accept();
    }
    else
        event->ignore();
}

void MainWindow::_SaveSettings(QSettings &settings) const
{
    settings.setValue("window_always_on_top", isAlwaysOnTop());
    settings.setValue("window_state", saveState());
    settings.setValue("window_geometry", saveGeometry());
    settings.setValue("working_directory", _currentDirectory);
    _editpane->saveSession(settings);
    _menubar->recentFilesList()->saveSettings(settings);
}

} // namespace fedup
