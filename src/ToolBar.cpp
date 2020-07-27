#include "ToolBar.h"
#include "Actions.h"

namespace fedup {

ToolBar::ToolBar(Actions *actions, QWidget *parent) : QToolBar(parent)
{
	setFloatable(false);
	setMovable(false);
	setIconSize(QSize(16, 16));

	addAction(actions->fileNew);
	addAction(actions->fileOpen);
	addAction(actions->fileSave);
	addAction(actions->fileClose);
	addAction(actions->fileCloseAll);
	addSeparator();
	addAction(actions->editUndo);
	addAction(actions->editRedo);
	addSeparator();
	addAction(actions->searchFind);
	addAction(actions->searchReplace);
	addSeparator();
	addAction(actions->macroStartRecording);
	addAction(actions->macroStopRecording);
	addAction(actions->macroPlayback);
	addAction(actions->macroRunMultiple);
}

ToolBar::~ToolBar()
{
}

} // namespace fedup
