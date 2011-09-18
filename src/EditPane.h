#ifndef FEDUP_EDITPANE_H
#define FEDUP_EDITPANE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QSettings;
class QString;
QT_END_NAMESPACE

namespace fedup {

class EditPaneTabs;
class FScintilla;
class TabContext;

class EditPane : public QWidget
{
	Q_OBJECT
public:
	EditPane(QWidget *parent = NULL);
	~EditPane();

	EditPaneTabs * tabs() const {return _tabs;}
	FScintilla * editor() const {return _editor;}

	void loadSession(QSettings &settings);
	void saveSession(QSettings &settings) const;

	bool open(const QString &filePath);
public slots:
	void openNew();
	void closeTab();
	void closeAll();
protected slots:
	void _slot_TabCloseRequested(int index);
	void _slot_TabRemoved(TabContext *context);
	void _slot_TabChanged(TabContext *context, TabContext *oldContext);
protected:
	EditPaneTabs *_tabs;
	FScintilla *_editor;
	static int _nextNewIndex;
};

} // namespace fedup

#endif
