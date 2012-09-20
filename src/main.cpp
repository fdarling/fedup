#include <QtSingleApplication>
#include <QFileInfo>
#include "MainWindow.h"

class SignalFunctor
{
public:
	SignalFunctor(QtSingleApplication &a) : app(a)
	{
	}
	void operator()(const QString &filePath)
	{
		app.sendMessage(filePath);
	}
protected:
	QtSingleApplication &app;
};

class OpenFunctor
{
public:
	OpenFunctor(fedup::MainWindow &w) : win(w)
	{
	}
	void operator()(const QString &filePath)
	{
		win.open(filePath);
	}
protected:
	fedup::MainWindow &win;
};

template <typename T>
static void openArguments(T functor)
{
	const QStringList arguments = qApp->arguments();
	const QString applicationFilename = QFileInfo(qApp->applicationFilePath()).fileName();
	const QString applicationBaseName = QFileInfo(qApp->applicationFilePath()).completeBaseName();
	QStringList::const_iterator it = arguments.begin();
	if (it != arguments.end() && (it->endsWith(applicationFilename, Qt::CaseInsensitive) || it->endsWith(applicationBaseName, Qt::CaseInsensitive)))
		++it;
	for (; it != arguments.end(); ++it)
		functor(QFileInfo(*it).absoluteFilePath());
}

int main(int argc, char **argv)
{
	QtSingleApplication app(argc, argv);
	if (app.isRunning())
	{
		app.activateWindow();
		openArguments(SignalFunctor(app));
		return 0;
	}

	fedup::MainWindow win;
	win.show();

	QObject::connect(&app, SIGNAL(messageReceived(const QString&)), &win, SLOT(open(const QString&)));
	openArguments(OpenFunctor(win));
	return app.exec();
}
