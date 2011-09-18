#include "ToolBar.h"
#include "Actions.h"

namespace fedup {

ToolBar::ToolBar(Actions *actions, QWidget *parent) : QToolBar(parent)
{
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
}

ToolBar::~ToolBar()
{
}

} // namespace fedup
