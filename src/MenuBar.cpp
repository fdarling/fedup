#include "MenuBar.h"
#include "Actions.h"

namespace fedup {

MenuBar::MenuBar(Actions *_actions, QWidget *parent) : QMenuBar(parent) // TODO rename _actions to actions
{
	{
		QMenu * const fileMenu = this->addMenu("&File");
		fileMenu->addAction(_actions->fileNew);
		// TODO "Re&load from Disk"
		fileMenu->addAction(_actions->fileOpen);
		fileMenu->addAction(_actions->fileSave);
		fileMenu->addAction(_actions->fileSaveAs);
		// TODO "Save a Copy As..."
		// TODO "Save All"
		// TODO "Rename..."
		fileMenu->addAction(_actions->fileClose);
		fileMenu->addAction(_actions->fileCloseAll);
		// TODO "Close All but Active Document"
		// TODO "Delete from Disk"
		fileMenu->addSeparator();
		fileMenu->addAction(_actions->fileExit);
	}
	{
		QMenu * const editMenu = this->addMenu("&Edit");
		editMenu->addAction(_actions->editUndo);
		editMenu->addAction(_actions->editRedo);
		editMenu->addSeparator();
		editMenu->addAction(_actions->editCut);
		editMenu->addAction(_actions->editCopy);
		editMenu->addAction(_actions->editPaste);
		editMenu->addAction(_actions->editDelete);
		// TODO /*, QKeySequence("Del")*/ have this display "DEL" next to it? I don't want to override normal DEL operation though... Notepad++ actually sends a delete keystroke!
		editMenu->addAction(_actions->editSelectAll);
		editMenu->addSeparator();
		editMenu->addAction(_actions->editDuplicateLines);
		editMenu->addAction(_actions->editDeleteLine);
		editMenu->addAction(_actions->editMoveLineUp);
		editMenu->addAction(_actions->editMoveLineDown);
		editMenu->addSeparator();
		editMenu->addAction(_actions->editToggleBlockComment);
		editMenu->addSeparator();
		editMenu->addAction(_actions->editTrimTrailingSpaces);


	}
	{
		QMenu * const searchMenu = this->addMenu("&Search");
		searchMenu->addAction(_actions->searchFind);
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
		searchMenu->addAction(_actions->searchGoTo);
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
