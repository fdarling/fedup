#include "RecentFilesList.h"

#include <QMenu>
#include <QActionGroup>
#include <QAction>
#include <QMenu>
#include <QSettings>

namespace fedup {

static const int MAX_RECENT_FILES = 13;

RecentFilesList::RecentFilesList(QMenu *ffileMenu, QSettings &settings) : fileMenu(ffileMenu), separator(fileMenu->addSeparator()), recentFilesGroup(NULL)
{
	// fileMenu->insertAction(separator, new QAction(QString("Testing 1 2 3"), this));
	recentFilesGroup = new QActionGroup(this);
	connect(recentFilesGroup, SIGNAL(triggered(QAction *)), this, SLOT(slot_RecentFileClicked(QAction *)));
	{
		const int recentArraySize = settings.beginReadArray("recent");
		for (int i = 0; i < recentArraySize; i++)
		{
			settings.setArrayIndex(i);
			const QString filePath = settings.value("filepath").toString();
			
			if (recentFilesMap.contains(filePath))
				continue;
			
			QAction * const recentFileAction = new QAction(/*QString::number(recentFilesList.size() + 1) + QString(": ") + */filePath, this);
			recentFileAction->setData(filePath);
			recentFilesGroup->addAction(recentFileAction);
			
			fileMenu->insertAction(separator, recentFileAction);
			recentFilesMap.insert(filePath, recentFileAction);
			recentFilesList.append(recentFileAction);
		}
		settings.endArray();
	}
	// connect(_recentlyUsedGroup, SIGNAL(triggered(QAction *)), this, SLOT(slot_FileRecentFileSelected(QAction *)));
}

RecentFilesList::~RecentFilesList()
{
}

void RecentFilesList::saveSettings(QSettings &settings)
{
	settings.beginWriteArray("recent");
	QActionList::const_iterator it = recentFilesList.begin();
	for (int i = 0; it != recentFilesList.end(); ++it)
	{
		const QAction * const action = *it;
		if (action == NULL)
			continue;
		settings.setArrayIndex(i);
		settings.setValue("filepath", action->data().toString());
		i++;
	}
	settings.endArray();
}

void RecentFilesList::slot_FileOpened(const QString &filePath)
{
	if (recentFilesMap.contains(filePath))
		return;
	
	QAction * const recentFileAction = new QAction(/*QString::number(recentFilesList.size() + 1) + QString(": ") + */filePath, this);
	recentFileAction->setData(filePath);
	recentFilesGroup->addAction(recentFileAction);
	
	fileMenu->insertAction(recentFilesList.empty() ? separator : recentFilesList.first(), recentFileAction);
	
	recentFilesMap.insert(filePath, recentFileAction);
	recentFilesList.prepend(recentFileAction);
	
	if (recentFilesList.size() > MAX_RECENT_FILES)
	{
		QAction * const action = recentFilesList.last();
		const QString filePath = action->data().toString();
		fileMenu->removeAction(action);
		recentFilesGroup->removeAction(action);
		recentFilesMap.remove(filePath);
		recentFilesList.removeLast();
		delete action;
	}
}

void RecentFilesList::slot_RecentFileClicked(QAction *action)
{
	emit(recentFileClicked(action->data().toString()));
}

} // namespace fedup
