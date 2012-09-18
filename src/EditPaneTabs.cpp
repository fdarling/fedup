#include "EditPaneTabs.h"
#include "LoadIcon.h"

#include <QVariant>
//#include <QDebug>

namespace fedup {

static QIcon savedIcon;
static QIcon unsavedIcon;

EditPaneTabs::EditPaneTabs(QWidget *parent) : QTabBar(parent), _lastIndex(-1)
{
	setExpanding(false);
	setMovable(true);
	setTabsClosable(true);

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(_slot_CurrentChanged(int)));
	connect(this, SIGNAL(tabMoved(int, int)), this, SLOT(_slot_TabMoved(int, int)));

	if (savedIcon.isNull())
	{
		savedIcon = LoadIcon("saved.ico");
		unsavedIcon = LoadIcon("unsaved.ico");
	}
}

EditPaneTabs::~EditPaneTabs()
{
}

int EditPaneTabs::addTab(const QString &tabname, TabContext *context)
{
	blockSignals(true);
	const int newIndex = QTabBar::addTab(savedIcon, tabname);
	setTabData(newIndex, qVariantFromValue(reinterpret_cast<void*>(context)));
	blockSignals(false);
	const bool shouldEmit = (count() == 1);
	if (currentIndex() == newIndex)
		emit currentChanged(newIndex);
	if (shouldEmit)
		emit containsTabsChanged(true);
	updateGeometry(); // HACK to workaround a Qt bug where the geometry is incorrectly cached and the QTabBar is invisible
	return newIndex;
}

TabContext * EditPaneTabs::tabContext(int index) const
{
	return reinterpret_cast<TabContext*>(tabData(index).value<void*>());
}

void EditPaneTabs::removeTab(int index)
{
	if (_lastIndex == index)
		_lastIndex = -1;
	else if (_lastIndex > index)
		_lastIndex--;
	//qDebug() << "About to remove tab" << index;
	TabContext * const context = tabContext(index);
	// TODO should I use blockSignals before actually removing the tab?
	QTabBar::removeTab(index);
	emit tabRemoved(context);
	if (count() == 0)
	{
		emit containsTabsChanged(false);
		emit tabChanged(NULL, NULL);
	}
	updateGeometry(); // HACK to workaround a Qt bug where the geometry is incorrectly cached and the QTabBar is invisible
	//qDebug() << "Removed tab with context =" << context;
	//qDebug() << "Done removing tab" << index;
}

void EditPaneTabs::slot_ModificationChanged(bool m)
{
	if (currentIndex() == -1)
		return;
	setTabIcon(currentIndex(), m ? unsavedIcon : savedIcon);
}

void EditPaneTabs::_slot_CurrentChanged(int index)
{
	if (index == _lastIndex)
		return;
	const int oldLastIndex = _lastIndex;
	_lastIndex = index;

	//qDebug() << "Changed tab index from " << oldLastIndex << "to" << index;
	emit tabChanged((index == -1) ? NULL : tabContext(index), (oldLastIndex == -1) ? NULL : tabContext(oldLastIndex));
}

void EditPaneTabs::_slot_TabMoved(int from, int to)
{
	Q_UNUSED(from);
	Q_UNUSED(to);
	//qDebug() << "_slot_TabMoved(" << from << "," << to << ")";
	_lastIndex = currentIndex();
}

} // namespace fedup
