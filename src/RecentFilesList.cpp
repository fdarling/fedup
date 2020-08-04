#include "RecentFilesList.h"

#include <QMenu>
#include <QActionGroup>
#include <QAction>
#include <QMenu>
#include <QSettings>

namespace fedup {

static const int MAX_RECENT_FILES = 13;

RecentFilesList::RecentFilesList(QMenu *ffileMenu, QSettings &settings, QObject *parent) : QObject(parent), _fileMenu(ffileMenu), _separator(_fileMenu->addSeparator()), _recentFilesGroup(NULL)
{
    // _fileMenu->insertAction(_separator, new QAction(QString("Testing 1 2 3"), this));
    _recentFilesGroup = new QActionGroup(this);
    connect(_recentFilesGroup, &QActionGroup::triggered, this, &RecentFilesList::slot_RecentFileClicked);
    {
        const int recentArraySize = settings.beginReadArray("recent");
        for (int i = 0; i < recentArraySize; i++)
        {
            settings.setArrayIndex(i);
            const QString filePath = settings.value("filepath").toString();

            if (_recentFilesMap.contains(filePath))
                continue;

            QAction * const recentFileAction = new QAction(/*QString::number(_recentFilesList.size() + 1) + QString(": ") + */filePath, this);
            recentFileAction->setData(filePath);
            _recentFilesGroup->addAction(recentFileAction);

            _fileMenu->insertAction(_separator, recentFileAction);
            _recentFilesMap.insert(filePath, recentFileAction);
            _recentFilesList.append(recentFileAction);
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
    QActionList::const_iterator it = _recentFilesList.begin();
    for (int i = 0; it != _recentFilesList.end(); ++it)
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
    if (_recentFilesMap.contains(filePath))
        return;

    QAction * const recentFileAction = new QAction(/*QString::number(_recentFilesList.size() + 1) + QString(": ") + */filePath, this);
    recentFileAction->setData(filePath);
    _recentFilesGroup->addAction(recentFileAction);

    _fileMenu->insertAction(_recentFilesList.empty() ? _separator : _recentFilesList.first(), recentFileAction);

    _recentFilesMap.insert(filePath, recentFileAction);
    _recentFilesList.prepend(recentFileAction);

    if (_recentFilesList.size() > MAX_RECENT_FILES)
    {
        QAction * const action = _recentFilesList.last();
        const QString filePath = action->data().toString();
        _fileMenu->removeAction(action);
        _recentFilesGroup->removeAction(action);
        _recentFilesMap.remove(filePath);
        _recentFilesList.removeLast();
        delete action;
    }
}

void RecentFilesList::slot_RecentFileClicked(QAction *action)
{
    emit recentFileClicked(action->data().toString());
}

} // namespace fedup
