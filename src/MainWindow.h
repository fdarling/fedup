#ifndef FEDUP_MAINWINDOW_H
#define FEDUP_MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QString>

namespace fedup {

class Actions;
class MenuBar;
class ToolBar;
class StatusBar;
class EditPane;
class FindDialog;
class GoToDialog;
class SearchResultsDock;
class TabContext;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = NULL);
	~MainWindow();
	bool isAlwaysOnTop() const;
signals:
	void sig_AlwaysOnTopChanged(bool onTop);
	void sig_FullscreenChanged(bool onTop);
public slots:
	void open();
	void open(const QString &filePath);
	bool save();
	bool saveAs();
	bool saveAs(const QString &filePath);
protected slots:
	void _slot_SearchGoTo();
	void _slot_TabChanged(TabContext *context, TabContext *oldContext);
	void _slot_SetAlwaysOnTop(bool onTop);
	void _slot_SetFullscreen(bool fullscreen);
protected:
	void _SetupActions();
	void _SetupConnections();

	void showEvent(QShowEvent *event);
	void closeEvent(QCloseEvent *event);

	QSettings _settings;
	QString _currentDirectory;
	Actions *_actions;
	MenuBar *_menubar;
	ToolBar *_toolbar;
	StatusBar *_statusbar;
	EditPane *_editpane;
	FindDialog *_findDialog;
	GoToDialog *_gotoDialog;
	SearchResultsDock *_searchResultsDock;
};

} // namespace fedup

#endif
