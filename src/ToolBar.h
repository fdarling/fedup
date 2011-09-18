#ifndef FEDUP_TOOLBAR_H
#define FEDUP_TOOLBAR_H

#include <QToolBar>

namespace fedup {

class Actions;

class ToolBar : public QToolBar
{
	Q_OBJECT
public:
	ToolBar(Actions *actions, QWidget *parent = NULL);
	~ToolBar();
};

} // namespace fedup

#endif
