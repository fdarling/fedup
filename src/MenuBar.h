#ifndef FEDUP_MENUBAR_H
#define FEDUP_MENUBAR_H

#include <QMenuBar>

namespace fedup {

class Actions;

class MenuBar : public QMenuBar
{
	Q_OBJECT
public:
	MenuBar(Actions *actions, QWidget *parent = NULL);
	~MenuBar();
};

} // namespace fedup

#endif
