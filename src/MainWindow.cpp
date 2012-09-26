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
	connect(e,    SIGNAL(modificationChanged(bool)), _actions->fileSave,     SLOT(setEnabled(bool)));
	connect(tabs, SIGNAL(containsTabsChanged(bool)), _actions->fileSaveAs,   SLOT(setEnabled(bool)));
	connect(tabs, SIGNAL(containsTabsChanged(bool)), _actions->fileClose,    SLOT(setEnabled(bool)));
	connect(tabs, SIGNAL(containsTabsChanged(bool)), _actions->fileCloseAll, SLOT(setEnabled(bool)));

	connect(_actions->fileNew, SIGNAL(triggered()), _editpane, SLOT(openNew()));
	connect(_actions->fileOpen, SIGNAL(triggered()), this, SLOT(open()));
	connect(_actions->fileSave, SIGNAL(triggered()), this, SLOT(save()));
	connect(_actions->fileSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
	connect(_actions->fileClose, SIGNAL(triggered()), _editpane, SLOT(closeTab()));
	connect(_actions->fileCloseAll, SIGNAL(triggered()), _editpane, SLOT(closeAll()));
	connect(_actions->fileExit, SIGNAL(triggered()), this, SLOT(close()));

	// make the actions know when they are available
	_actions->editUndo->setEnabled(false);
	_actions->editRedo->setEnabled(false);
	_actions->editCut->setEnabled(false);
	_actions->editCopy->setEnabled(false);
	_actions->editDelete->setEnabled(false);
	connect(e, SIGNAL(undoAvailable(bool)), _actions->editUndo, SLOT(setEnabled(bool)));
	connect(e, SIGNAL(redoAvailable(bool)), _actions->editRedo, SLOT(setEnabled(bool)));
	connect(e, SIGNAL(copyAvailable(bool)), _actions->editCut, SLOT(setEnabled(bool)));
	connect(e, SIGNAL(copyAvailable(bool)), _actions->editCopy, SLOT(setEnabled(bool)));
	connect(e, SIGNAL(copyAvailable(bool)), _actions->editDelete, SLOT(setEnabled(bool)));

	connect(_actions->editUndo, SIGNAL(triggered()), e, SLOT(undo()));
	connect(_actions->editRedo, SIGNAL(triggered()), e, SLOT(redo()));
	connect(_actions->editCut, SIGNAL(triggered()), e, SLOT(cut()));
	connect(_actions->editCopy, SIGNAL(triggered()), e, SLOT(copy()));
	connect(_actions->editPaste, SIGNAL(triggered()), e, SLOT(paste()));
	connect(_actions->editDelete, SIGNAL(triggered()), e, SLOT(removeSelectedText()));
	connect(_actions->editSelectAll, SIGNAL(triggered()), e, SLOT(selectAll()));
	connect(_actions->editDuplicateLines, SIGNAL(triggered()), e, SLOT(duplicateLines()));
	connect(_actions->editDeleteLine, SIGNAL(triggered()), e, SLOT(deleteLine()));
	connect(_actions->editUndeleteLines, SIGNAL(triggered()), e, SLOT(undeleteLines()));
	connect(_actions->editMoveLineUp, SIGNAL(triggered()), e, SLOT(moveLineUp()));
	connect(_actions->editMoveLineDown, SIGNAL(triggered()), e, SLOT(moveLineDown()));
	connect(_actions->editToggleBlockComment, SIGNAL(triggered()), e, SLOT(toggleCommented()));
	connect(_actions->editTrimTrailingSpaces, SIGNAL(triggered()), e, SLOT(trimTrailingWhitespace()));
	connect(_actions->editSimplifyWhitespace, SIGNAL(triggered()), e, SLOT(simplifyWhitespace()));
	_slot_EolModeChanged(e->eolMode()); // set the initial value
	connect(_actions->editEolGroup, SIGNAL(triggered(QAction*)), this, SLOT(_slot_EditEolModeTriggered(QAction*)));

	connect(_actions->searchFind, SIGNAL(triggered()), _findDialog, SLOT(showFind()));
	connect(_actions->searchFindInFiles, SIGNAL(triggered()), _findDialog, SLOT(showFindInFiles()));
	connect(_actions->searchFindNext, SIGNAL(triggered()), _findDialog, SLOT(findNext()));
	connect(_actions->searchFindPrev, SIGNAL(triggered()), _findDialog, SLOT(findPrev()));
	connect(_actions->searchReplace, SIGNAL(triggered()), _findDialog, SLOT(showReplace()));
	connect(_actions->searchGoTo, SIGNAL(triggered()), this, SLOT(_slot_SearchGoTo()));
	connect(_actions->searchBookmarkToggle, SIGNAL(triggered()), e, SLOT(toggleBookmark()));
	connect(_actions->searchBookmarkNext, SIGNAL(triggered()), e, SLOT(findNextBookmark()));
	connect(_actions->searchBookmarkPrev, SIGNAL(triggered()), e, SLOT(findPrevBookmark()));
	connect(_actions->searchBookmarkClearAll, SIGNAL(triggered()), e, SLOT(clearBookmarks()));

	// make the actions know when they are available
	_actions->viewSearchResults->setChecked(_searchResultsDock->isVisible());
	// _actions->viewFullscreen->setChecked(isFullScreen()); // TODO figure out why this doesn't fully work
	// _actions->viewFullscreen->setChecked(windowState().testFlag(Qt::WindowFullScreen)); // TODO figure out if this is better than the above method, NOTE: this doesn't seem to work, I have to use showEvent to track it instead...
	_actions->viewAlwaysOnTop->setChecked(isAlwaysOnTop());
	_actions->viewSymbolWhitespace->setChecked(e->whitespaceVisibility() != QsciScintilla::WsInvisible);
	_actions->viewSymbolNewlines->setChecked(e->eolVisibility());
	_actions->viewIndentationGuide->setChecked(e->indentationGuides());
	connect(_searchResultsDock, SIGNAL(visibilityChanged(bool)), _actions->viewSearchResults, SLOT(setChecked(bool)));
	connect(this, SIGNAL(sig_AlwaysOnTopChanged(bool)), _actions->viewAlwaysOnTop, SLOT(setChecked(bool)));
	connect(this, SIGNAL(sig_FullscreenChanged(bool)), _actions->viewFullscreen, SLOT(setChecked(bool)));

	connect(_actions->viewAlwaysOnTop, SIGNAL(toggled(bool)), this, SLOT(_slot_SetAlwaysOnTop(bool)));
	connect(_actions->viewFullscreen, SIGNAL(toggled(bool)), this, SLOT(_slot_SetFullscreen(bool)));
	connect(_actions->viewSymbolWhitespace, SIGNAL(toggled(bool)), e, SLOT(setWhitespaceVisible(bool)));
	connect(_actions->viewIndentationGuide, SIGNAL(toggled(bool)), e, SLOT(setIndentationGuides(bool)));
	connect(_actions->viewSymbolNewlines, SIGNAL(toggled(bool)), e, SLOT(setEolVisibility(bool)));
	connect(_actions->viewSearchResults, SIGNAL(toggled(bool)), _searchResultsDock, SLOT(setVisible(bool)));

	// make the actions know when they are available
	_actions->macroStartRecording->setEnabled(true);
	_actions->macroStopRecording->setEnabled(false);
	_actions->macroPlayback->setEnabled(false);
	_actions->macroRunMultiple->setEnabled(false);
	connect(_actions->macroStartRecording, SIGNAL(triggered()), this, SLOT(_slot_MacroStartRecording()));
	connect(_actions->macroStopRecording, SIGNAL(triggered()), this, SLOT(_slot_MacroStopRecording()));
	connect(_actions->macroPlayback, SIGNAL(triggered()), this, SLOT(_slot_MacroPlayback()));
	connect(_actions->macroRunMultiple, SIGNAL(triggered()), this, SLOT(_slot_MacroRunMultiple()));
}

void MainWindow::_SetupConnections()
{
	FScintilla * const e = _editpane->editor();
	EditPaneTabs * const tabs = _editpane->tabs();

	connect(e, SIGNAL(cursorPositionChanged(int, int)), _statusbar, SLOT(slot_SetCursorPosition(int, int)));
	connect(e, SIGNAL(selectionLengthChanged(int)), _statusbar, SLOT(slot_SetSelectionLength(int)));
	connect(e, SIGNAL(lineCountChanged(int)), _statusbar, SLOT(slot_SetLineCount(int)));
	connect(e, SIGNAL(lengthChanged(int)), _statusbar, SLOT(slot_SetLength(int)));
	connect(e, SIGNAL(lineChanged(int)), _gotoDialog, SLOT(slot_SetCurrentLine(int)));
	connect(e, SIGNAL(positionChanged(int)), _gotoDialog, SLOT(slot_SetCurrentOffset(int)));
	connect(e, SIGNAL(lineCountChanged(int)), _gotoDialog, SLOT(slot_SetMaxLine(int)));
	connect(e, SIGNAL(lengthChanged(int)), _gotoDialog, SLOT(slot_SetMaxOffset(int))); // TODO use slot_SetLength instead?
	connect(e, SIGNAL(eolModeChanged(FScintilla::EolMode)), this, SLOT(_slot_EolModeChanged(FScintilla::EolMode)));
	connect(_gotoDialog, SIGNAL(goToLine(int)), e, SLOT(goToLine(int)));
	connect(_gotoDialog, SIGNAL(goToOffset(int)), e, SLOT(goToOffset(int)));

	connect(_menubar->recentFilesList(), SIGNAL(recentFileClicked(const QString &)), this, SLOT(open(const QString &)));

	connect(tabs, SIGNAL(tabChanged(TabContext *, TabContext *)), this, SLOT(_slot_TabChanged(TabContext *, TabContext *)));

	connect(_findDialog, SIGNAL(sig_SearchStarted(const QString &)), _searchResultsDock, SLOT(startSearch(const QString &)));
	connect(_findDialog, SIGNAL(sig_ResultFound(const QString &, int, const QString &, int, int)), _searchResultsDock, SLOT(addResult(const QString &, int, const QString &, int, int)));
	connect(_findDialog, SIGNAL(sig_SearchEnded()), _searchResultsDock, SLOT(endSearch()));

	connect(_searchResultsDock, SIGNAL(requestOpenFileLine(const QString &, int)), this, SLOT(_slot_OpenFileLine(const QString &, int)));
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
		// connect(_editorMacro, SIGNAL(destroyed()), this, SLOT(_slot_MacroDestroyed()));
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
		// connect(_editorMacro, SIGNAL(destroyed()), this, SLOT(slot_MacroDestroyed()));
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
			// connect(_editorMacro, SIGNAL(destroyed()), this, SLOT(slot_MacroDestroyed()));
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

void MainWindow::showEvent(QShowEvent *event)
{
	_actions->viewFullscreen->setChecked(isFullScreen());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	ExitSaveDialog exitSaveDialog(this);
	if (exitSaveDialog.shouldClose(this, _editpane))
	{
		_settings.setValue("window_always_on_top", isAlwaysOnTop());
		_settings.setValue("window_state", saveState());
		_settings.setValue("window_geometry", saveGeometry());
		_settings.setValue("working_directory", _currentDirectory);
		_editpane->saveSession(_settings);
		_menubar->recentFilesList()->saveSettings(_settings);
		event->accept();
	}
	else
		event->ignore();
}

} // namespace fedup
