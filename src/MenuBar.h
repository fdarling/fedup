#ifndef FEDUP_MENUBAR_H
#define FEDUP_MENUBAR_H

#include <QMenuBar>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace fedup {

class Actions;
class RecentFilesList;

class MenuBar : public QMenuBar
{
	Q_OBJECT
public:
	MenuBar(Actions *actions, QSettings &settings, QWidget *parent = NULL);
	~MenuBar();

	RecentFilesList * recentFilesList() const {return _recentFilesList;}
protected:
	RecentFilesList *_recentFilesList;
};

} // namespace fedup

#endif
