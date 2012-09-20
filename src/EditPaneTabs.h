#ifndef FEDUP_EDITPANETABS_H
#define FEDUP_EDITPANETABS_H

#include <QTabBar>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

namespace fedup {

class TabContext;

class EditPaneTabs : public QTabBar
{
	Q_OBJECT
public:
	EditPaneTabs(QWidget *parent = NULL);
	~EditPaneTabs();

	int addTab(const QString &tabname, TabContext *context);
	TabContext * tabContext(int index) const;

	void removeTab(int index);
public slots:
	void slot_ModificationChanged(bool m);
signals:
	void containsTabsChanged(bool hasTabs);
	void tabRemoved(TabContext *context);
	void tabChanged(TabContext *context, TabContext *oldContext);
	void tabCloseAllButRequested(int index);
protected slots:
	void _slot_CurrentChanged(int index);
	void _slot_TabMoved(int from, int to);
protected:
	void mousePressEvent(QMouseEvent *event);
	int _lastIndex;
};

} // namespace fedup

#endif
