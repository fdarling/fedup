#include <QtSingleApplication>
#include <QFileInfo>
#include "MainWindow.h"

int main(int argc, char **argv)
{
	QtSingleApplication app(argc, argv);
	if (app.isRunning())
	{
		// TODO eliminate duplicate code, it's a tough one though :-/
		const QStringList arguments = app.arguments();
		const QString applicationFilename = QFileInfo(app.applicationFilePath()).fileName();
		const QString applicationBaseName = QFileInfo(app.applicationFilePath()).completeBaseName();
		QStringList::const_iterator it = arguments.begin();
		if (it != arguments.end() && (it->endsWith(applicationFilename, Qt::CaseInsensitive) || it->endsWith(applicationBaseName, Qt::CaseInsensitive)))
			++it;
		for (; it != arguments.end(); ++it)
			app.sendMessage(*it);
		return 0;
	}

	fedup::MainWindow win;
	win.show();

	app.setActivationWindow(&win);
	QObject::connect(&app, SIGNAL(messageReceived(const QString&)), &win, SLOT(open(const QString&)));
	{
		const QStringList arguments = app.arguments();
		const QString applicationFilename = QFileInfo(app.applicationFilePath()).fileName();
		const QString applicationBaseName = QFileInfo(app.applicationFilePath()).completeBaseName();
		QStringList::const_iterator it = arguments.begin();
		if (it != arguments.end() && (it->endsWith(applicationFilename, Qt::CaseInsensitive) || it->endsWith(applicationBaseName, Qt::CaseInsensitive)))
			++it;
		for (; it != arguments.end(); ++it)
			win.open(*it);
	}
	return app.exec();
}
