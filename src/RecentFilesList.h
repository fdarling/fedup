#ifndef FEDUP_RECENTFILESLIST_H
#define FEDUP_RECENTFILESLIST_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>
class QActionGroup;

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QSettings;
QT_END_NAMESPACE

namespace fedup {

class RecentFilesList : public QObject
{
    Q_OBJECT
public:
    RecentFilesList(QMenu *ffileMenu, QSettings &ssettings, QObject *parent = NULL);
    ~RecentFilesList();

    void saveSettings(QSettings &settings);
signals:
    void recentFileClicked(const QString &filePath);
public slots:
    void slot_FileOpened(const QString &filePath);
protected slots:
    void slot_RecentFileClicked(QAction *action);
protected:
    typedef QAction* QActionPointer;
    typedef QMap<QString, QActionPointer> PathToActionMap;
    typedef QList<QActionPointer> QActionList;

    QMenu * const _fileMenu;
    QActionPointer _separator;
    QActionGroup * _recentFilesGroup;
    PathToActionMap _recentFilesMap;
    QActionList _recentFilesList;
};

} // namespace fedup

#endif
