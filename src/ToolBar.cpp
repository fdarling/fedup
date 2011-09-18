#include "ToolBar.h"
#include "Actions.h"

namespace fedup {

ToolBar::ToolBar(Actions *_actions, QWidget *parent) : QToolBar(parent) // TODO rename _actions to actions
{
	setMovable(false);
	setIconSize(QSize(16, 16));

	addAction(_actions->fileNew);
	addAction(_actions->fileOpen);
	addAction(_actions->fileSave);
	addAction(_actions->fileClose);
	addAction(_actions->fileCloseAll);
	addSeparator();
	addAction(_actions->editUndo);
	addAction(_actions->editRedo);
	addSeparator();
	addAction(_actions->searchFind);
}

ToolBar::~ToolBar()
{
}

} // namespace fedup
