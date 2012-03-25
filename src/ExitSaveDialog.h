#ifndef FEDUP_EXITSAVEDIALOG_H
#define FEDUP_EXITSAVEDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QTableWidget;
class QModelIndex;
QT_END_NAMESPACE

namespace fedup {

class MainWindow;
class EditPane;
// class TabContext;

class ExitSaveDialog : public QDialog
{
	Q_OBJECT
public:
	ExitSaveDialog(QWidget *parent = NULL);
	~ExitSaveDialog();

	bool shouldClose(MainWindow *mainWindow, EditPane *editPane);
protected slots:
	void _slot_Save();
	void _slot_Skip();
	void _slot_CheckAll();
	void _slot_UncheckAll();
	void _slot_SaveSelected();
	void _slot_RowActivated(const QModelIndex &index);
protected:
	QTableWidget *_table;
	MainWindow *_mainWindow;
	EditPane *_editPane;
};

} // namespace fedup

#endif
