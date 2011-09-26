#include "Actions.h"
#include "LoadIcon.h"

#include <QAction>

namespace fedup {

QAction * Actions::GenAction(const QString &name, const QString &seq, const QString &iconFilename)
{
	QAction * const action = new QAction(LoadIcon(iconFilename), name, this);
	action->setShortcut(QKeySequence(seq));
	return action;
}

Actions::Actions(QObject *parent) : QObject(parent)
{
	fileNew      = GenAction("&New",        "Ctrl+N",     "newFile.bmp");
	fileOpen     = GenAction("&Open",       "Ctrl+O",     "openFile.bmp");
	fileSave     = GenAction("&Save",       "Ctrl+S",     "saved.ico");
	fileSaveAs   = GenAction("Save &As...", "Ctrl+Alt+S", "");
	fileClose    = GenAction("&Close",      "Ctrl+W",     "closeFile.bmp");
	fileCloseAll = GenAction("Clos&e All",  "",           "closeAll.bmp");
	fileExit     = GenAction("E&xit",       "Alt+F4",     ""); // TODO "style()->standardIcon(QStyle::SP_TitleBarCloseButton)" and "QKeySequence::Quit"

	editUndo               = GenAction("&Undo",                  "Ctrl+Z",          "undo.bmp");
	editRedo               = GenAction("&Redo",                  "Ctrl+Alt+Z",      "redo.bmp");
	editCut                = GenAction("Cu&t",                   "Ctrl+X",          "cut.bmp");
	editCopy               = GenAction("&Copy",                  "Ctrl+C",          "copy.bmp");
	editPaste              = GenAction("&Paste",                 "Ctrl+V",          "paste.bmp");
	editDelete             = GenAction("&Delete",                "",                "delete.ico");
	editSelectAll          = GenAction("Select A&ll",            "Ctrl+A",          "");
	editDuplicateLines     = GenAction("Duplicate current line", "Ctrl+D",          "");
	editDeleteLine         = GenAction("Delete current line",    "Ctrl+K",          "");
	editMoveLineUp         = GenAction("Move Up Current Line",   "Ctrl+Shift+Up",   "");
	editMoveLineDown       = GenAction("Move Down Current Line", "Ctrl+Shift+Down", "");
	editToggleBlockComment = GenAction("Toggle Block Comment",   "Ctrl+Q",          "");
	editTrimTrailingSpaces = GenAction("Trim trailing spaces",   "",                "");

	searchFind = GenAction("&Find...", "Ctrl+F", "find.bmp");
	searchFindInFiles = GenAction("Find in Files...", "Ctrl+Shift+F", "");
	searchReplace = GenAction("&Replace...", "Ctrl+R", "findReplace.bmp");
	searchGoTo = GenAction("&Go to...", "Ctrl+G", "");
}

Actions::~Actions()
{
}

} // namespace fedup
