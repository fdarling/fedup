#include "MenuBar.h"
#include "Actions.h"
#include "RecentFilesList.h"

namespace fedup {

MenuBar::MenuBar(Actions *actions, QSettings &settings, QWidget *parent) : QMenuBar(parent), _recentFilesList(NULL)
{
	{
		QMenu * const fileMenu = this->addMenu("&File");
		fileMenu->addAction(actions->fileNew);
		// TODO "Re&load from Disk"
		fileMenu->addAction(actions->fileOpen);
		fileMenu->addAction(actions->fileSave);
		fileMenu->addAction(actions->fileSaveAs);
		// TODO "Save a Copy As..."
		// TODO "Save All"
		// TODO "Rename..."
		fileMenu->addAction(actions->fileClose);
		fileMenu->addAction(actions->fileCloseAll);
		// TODO "Close All but Active Document"
		// TODO "Delete from Disk"
		fileMenu->addSeparator();
		_recentFilesList = new RecentFilesList(fileMenu, settings);
		fileMenu->addSeparator();
		fileMenu->addAction(actions->fileExit);
	}
	{
		QMenu * const editMenu = this->addMenu("&Edit");
		editMenu->addAction(actions->editUndo);
		editMenu->addAction(actions->editRedo);
		editMenu->addSeparator();
		editMenu->addAction(actions->editCut);
		editMenu->addAction(actions->editCopy);
		editMenu->addAction(actions->editPaste);
		editMenu->addAction(actions->editDelete);
		// TODO /*, QKeySequence("Del")*/ have this display "DEL" next to it? I don't want to override normal DEL operation though... Notepad++ actually sends a delete keystroke!
		editMenu->addAction(actions->editSelectAll);
		editMenu->addSeparator();
		editMenu->addAction(actions->editDuplicateLines);
		editMenu->addAction(actions->editDeleteLine);
		editMenu->addAction(actions->editMoveLineUp);
		editMenu->addAction(actions->editMoveLineDown);
		editMenu->addSeparator();
		editMenu->addAction(actions->editToggleBlockComment);
		editMenu->addSeparator();
		editMenu->addAction(actions->editTrimTrailingSpaces);


	}
	{
		QMenu * const searchMenu = this->addMenu("&Search");
		searchMenu->addAction(actions->searchFind);
		// searchMenu->addAction("Find in Files...", this, SLOT(slot_SearchFindInFiles()), QKeySequence("Ctrl+Shift+F"));
// searchMenu->addAction("Find Next", tabs, SLOT(slot_FindNext()), QKeySequence("F3"));
// searchMenu->addAction("Find Previous", tabs, SLOT(slot_FindPrev()), QKeySequence("Shift+F3"));
		// searchMenu->addAction("Select and Find Next", tabs, SLOT(slot_FindPrev()), QKeySequence("Ctrl+F3"));
		// searchMenu->addAction("Select and Find Previous", tabs, SLOT(slot_FindPrev()), QKeySequence("Ctrl+Shift+F3"));
		// searchMenu->addAction("Find (Volatile) Next", tabs, SLOT(slot_FindPrev()), QKeySequence("Ctrl+Alt+F3"));
		// searchMenu->addAction("Find (Volatile) Previous", tabs, SLOT(slot_FindPrev()), QKeySequence("Ctrl+Alt+Shift+F3"));
	//searchMenu->addAction("Replace...", tabs, SLOT(slot_SearchReplace()), QKeySequence("Ctrl+H"));
		// searchMenu->addAction("Incremental Search", tabs, SLOT(slot_FindPrev()), QKeySequence("Ctrl+Alt+I"));
		// searchMenu->addAction("Search Results Window", tabs, SLOT(slot_FindPrev()), QKeySequence("F7"));
		// searchMenu->addAction("Next Search Result", tabs, SLOT(slot_FindPrev()), QKeySequence("F4"));
		// searchMenu->addAction("Previous Search Result", tabs, SLOT(slot_FindPrev()), QKeySequence("Shift+F4"));
		searchMenu->addAction(actions->searchGoTo);
		//searchMenu->addAction("Go to Matching Brace", this, SLOT(slot_SearchGoTo()), QKeySequence("Ctrl+B"));
	}
	{
		QMenu * const viewMenu = this->addMenu("&View");
		// viewMenu->addAction("Always on Top", this, SLOT(setAlwaysOnTop(bool)))->setCheckable(true); // TODO reenable
		// viewMenu->addAction("Toggle Full Screen Mode", this, SLOT(setFullscreen(bool)), QKeySequence("F11"))->setCheckable(true);
		// viewMenu->addAction("Post-It", this, SLOT(setPostit(bool)), QKeySequence("F12"))->setCheckable(true);
	}
}

MenuBar::~MenuBar()
{
}

} // namespace fedup
