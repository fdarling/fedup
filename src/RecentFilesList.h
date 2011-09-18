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
	RecentFilesList(QMenu *ffileMenu, QSettings &ssettings);
	~RecentFilesList();

	void saveSettings(QSettings &settings);
signals:
	void recentFileClicked(const QString &filePath);
public slots:
	void slot_FileOpened(const QString &filePath);
protected slots:
	void slot_RecentFileClicked(QAction *action);
protected:
	QMenu * const fileMenu;
	typedef QAction* QActionPointer;
	QActionPointer separator;
	QActionGroup * recentFilesGroup;
	typedef QMap<QString, QActionPointer> PathToActionMap;
	typedef QList<QActionPointer> QActionList;
	PathToActionMap recentFilesMap;
	QActionList recentFilesList;
};

} // namespace fedup

#endif
