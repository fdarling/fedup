#include "EditPaneTabs.h"
#include "TabContext.h"
#include "LoadIcon.h"

#include <QApplication>
#include <QClipboard>
#include <QFileInfo>
#include <QUrl>
#include <QMouseEvent>
#include <QMenu>
#include <QVariant>
//#include <QDebug>

#if   defined(Q_OS_WIN)
#include <QProcess>
#elif defined(Q_OS_MAC)
#include <QProcess>
#include <QStringList>
#else
#include <QDesktopServices>
#endif

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

void EditPaneTabs::mousePressEvent(QMouseEvent *event)
{
	if (event->button() != Qt::RightButton)
		return QTabBar::mousePressEvent(event);

	// compute the tab number
	const QPoint position = event->pos();
	const int clickedTabIndex = tabAt(position);

	if (clickedTabIndex == -1)
		return QTabBar::mousePressEvent(event);

	QMenu menu;
	QAction * const closeAction        = menu.addAction("Close");
	QAction * const closeAllButAction  = menu.addAction("Close All BUT This");
	// QAction * const saveAction         = menu.addAction("Save");
	// QAction * const saveAsAction       = menu.addAction("Save As");
	// QAction * const renameAction       = menu.addAction("Rename");
	// QAction * const deleteAction       = menu.addAction("Delete");
	menu.addSeparator();
	QAction * const showFolderAction   = menu.addAction("Show Containing Folder");
	menu.addSeparator();
	QAction * const fullFilePathAction = menu.addAction("Full File Path to Clipboard");
	QAction * const filenameAction     = menu.addAction("Filename to Clipboard");
	QAction * const directoryAction    = menu.addAction("Directory Path to Clipboard");

	QAction * const picked = menu.exec(event->globalPos());

	if (picked == NULL)
	{
		event->ignore();
		return;
	}
	event->accept();
	if (picked == closeAction)
	{
		emit tabCloseRequested(clickedTabIndex);
	}
	else if (picked == closeAllButAction)
	{
		emit tabCloseAllButRequested(clickedTabIndex);
	}
	else if (picked == showFolderAction)
	{
		const QString filePath = tabContext(clickedTabIndex)->filePath;
		// TODO move this into a class or function somewhere
#if   defined(Q_OS_WIN)
		QProcess::startDetached("explorer.exe", QStringList("/select," + QFileInfo(filePath).fileName()), QFileInfo(filePath).absolutePath());
#elif defined(Q_OS_MAC)
		QStringList scriptArgs;
		scriptArgs << "-e" << QString("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(QFileInfo(filePath).absoluteFilePath());
		QProcess::execute("/usr/bin/osascript", scriptArgs);
		scriptArgs.clear();
		scriptArgs << "-e" << "tell application \"Finder\" to activate";
		QProcess::execute("/usr/bin/osascript", scriptArgs);
#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(filePath).absolutePath()));
#endif
	}
	else if (picked == fullFilePathAction)
	{
		QApplication::clipboard()->setText(QFileInfo(tabContext(clickedTabIndex)->filePath).absoluteFilePath());
	}
	else if (picked == filenameAction)
	{
		QApplication::clipboard()->setText(QFileInfo(tabContext(clickedTabIndex)->filePath).fileName());
	}
	else if (picked == directoryAction)
	{
		QApplication::clipboard()->setText(QFileInfo(tabContext(clickedTabIndex)->filePath).absolutePath());
	}
}

} // namespace fedup
