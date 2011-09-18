#include "MainWindow.h"
#include "Actions.h"
#include "MenuBar.h"
#include "ToolBar.h"
#include "StatusBar.h"
#include "EditPane.h"
#include "EditPaneTabs.h"
#include "FScintilla.h"
#include "TabContext.h"
#include "FindDialog.h"
#include "GoToDialog.h"
#include "RecentFilesList.h"

#include <QFileDialog>
#include <QMessageBox>

namespace fedup {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _settings(QSettings::UserScope, "forestdarling.com", "fedup3"), _actions(NULL), _menubar(NULL), _toolbar(NULL), _statusbar(NULL), _editpane(NULL), _findDialog(NULL), _gotoDialog(NULL)
{
	_actions = new Actions(this);
	setMenuBar(_menubar = new MenuBar(_actions, _settings, this));
	addToolBar(_toolbar = new ToolBar(_actions, this));
	setStatusBar(_statusbar = new StatusBar(this));
	setCentralWidget(_editpane = new EditPane(this));

	_findDialog = new FindDialog(this);
	_gotoDialog = new GoToDialog(this);

	_SetupActions();
	_SetupConnections();

	if (!restoreGeometry(_settings.value("window_geometry").toByteArray()))
		setGeometry(QRect(50, 50, 800, 600));
	_currentDirectory = _settings.value("working_directory").toString();

	// reopen the previously open files
	_editpane->loadSession(_settings);
}

MainWindow::~MainWindow()
{
	_settings.setValue("window_geometry", saveGeometry());
	_settings.setValue("working_directory", _currentDirectory);
	_editpane->saveSession(_settings);
	_menubar->recentFilesList()->saveSettings(_settings);
}

void MainWindow::_SetupActions()
{
	FScintilla * const e = _editpane->editor();

	// make the actions know when they are available
	_actions->fileSave->setEnabled(false);
	connect(e, SIGNAL(modificationChanged(bool)), _actions->fileSave, SLOT(setEnabled(bool)));

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
	connect(_actions->editMoveLineUp, SIGNAL(triggered()), e, SLOT(moveLineUp()));
	connect(_actions->editMoveLineDown, SIGNAL(triggered()), e, SLOT(moveLineDown()));
	connect(_actions->editToggleBlockComment, SIGNAL(triggered()), e, SLOT(toggleCommented()));
	connect(_actions->editTrimTrailingSpaces, SIGNAL(triggered()), e, SLOT(trimTrailingWhitespace()));

	connect(_actions->searchFind, SIGNAL(triggered()), this, SLOT(_slot_SearchFind()));
	connect(_actions->searchGoTo, SIGNAL(triggered()), this, SLOT(_slot_SearchGoTo()));
}

void MainWindow::_SetupConnections()
{
	FScintilla * const e = _editpane->editor();

	connect(e, SIGNAL(cursorPositionChanged(int, int)), _statusbar, SLOT(slot_SetCursorPosition(int, int)));
	connect(e, SIGNAL(selectionLengthChanged(int)), _statusbar, SLOT(slot_SetSelectionLength(int)));
	connect(e, SIGNAL(lineCountChanged(int)), _statusbar, SLOT(slot_SetLineCount(int)));
	connect(e, SIGNAL(lengthChanged(int)), _statusbar, SLOT(slot_SetLength(int)));
	connect(e, SIGNAL(lineChanged(int)), _gotoDialog, SLOT(slot_SetCurrentLine(int)));
	connect(e, SIGNAL(positionChanged(int)), _gotoDialog, SLOT(slot_SetCurrentOffset(int)));
	connect(e, SIGNAL(lineCountChanged(int)), _gotoDialog, SLOT(slot_SetMaxLine(int)));
	connect(e, SIGNAL(lengthChanged(int)), _gotoDialog, SLOT(slot_SetMaxOffset(int))); // TODO use slot_SetLength instead?
	connect(_gotoDialog, SIGNAL(goToLine(int)), e, SLOT(goToLine(int)));
	connect(_gotoDialog, SIGNAL(goToOffset(int)), e, SLOT(goToOffset(int)));

	connect(_menubar->recentFilesList(), SIGNAL(recentFileClicked(const QString &)), this, SLOT(open(const QString &)));
}

void MainWindow::open()
{
	const QStringList fileList = QFileDialog::getOpenFileNames(this, "Open...", _currentDirectory);
	for (QStringList::const_iterator it = fileList.begin(); it != fileList.end(); ++it)
		open(*it);
}

void MainWindow::open(const QString &filePath)
{
	QFileInfo info(filePath);
	_currentDirectory = info.absolutePath();
	_editpane->open(filePath);
	_menubar->recentFilesList()->slot_FileOpened(filePath); // TODO use signals to communicate that a file opened?
}

void MainWindow::save()
{
	EditPaneTabs * const tabs = _editpane->tabs();
	if (tabs->count() == 0)
		return;
	TabContext * const context = tabs->tabContext(tabs->currentIndex());
	if (context->filePath.size() == 0)
	{
		saveAs();
		return;
	}
	QFile file(context->filePath);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox msg;
		msg.setText("Error opening \"" + context->filePath + "\" for writing");
		msg.exec();
		return;
	}
	FScintilla * const editor = _editpane->editor();
	if (!editor->write(&file))
	{
		QMessageBox msg;
		msg.setText("Error writing \"" + context->filePath + "\"");
		msg.exec();
		return;
	}
	editor->setModified(false);
}

void MainWindow::saveAs()
{
	const QString filename = QFileDialog::getSaveFileName(this, "Save As...", _currentDirectory);
	if (filename.size() == 0)
		return;

	saveAs(filename);
}

void MainWindow::saveAs(const QString &filePath)
{
	QFileInfo info(filePath);
	EditPaneTabs * const tabs = _editpane->tabs();
	FScintilla * const editor = _editpane->editor();
	if (tabs->count() == 0)
		return;
	TabContext * const context = tabs->tabContext(tabs->currentIndex());
	if (filePath != context->filePath)
	{
		context->filePath = filePath;
		tabs->setTabText(tabs->currentIndex(), info.fileName());
		editor->setModified(true);
	}
	QFile file(context->filePath);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox msg;
		msg.setText("Error opening \"" + filePath + "\" for writing");
		msg.exec();
		return;
	}
	if (!editor->write(&file))
	{
		QMessageBox msg;
		msg.setText("Error writing \"" + filePath + "\"");
		msg.exec();
		return;
	}
	editor->setModified(false);
	_currentDirectory = info.absolutePath();
}

void MainWindow::_slot_SearchFind()
{
	_findDialog->show();
	_findDialog->raise();
	_findDialog->activateWindow();
}

void MainWindow::_slot_SearchGoTo()
{
	_gotoDialog->show();
	_gotoDialog->raise();
	_gotoDialog->activateWindow();
}

} // namespace fedup
