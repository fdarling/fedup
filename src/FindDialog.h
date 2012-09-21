#ifndef FEDUP_FINDDIALOG_H
#define FEDUP_FINDDIALOG_H

#include <QDialog>
#include <QRect>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class QRadioButton;
class QTabBar;
// class QPushButton;
// class QGridLayout;
QT_END_NAMESPACE

namespace fedup {

class FScintilla;

class FindDialog : public QDialog
{
	Q_OBJECT
public:
	FindDialog(FScintilla *editor, QWidget *parent = NULL);
	~FindDialog();
public slots:
	void showFind();
	void showReplace();
	void showMark();
	void showFindInFiles();
	void findNext();
	void findPrev();
signals:
	void sig_SearchStarted(const QString &term);
	void sig_ResultFound(const QString &filePath, int line, const QString &result, int highlightStart, int highlightLength);
	void sig_SearchEnded();
protected slots:
	void _slot_FindNext();
	void _slot_Replace();
	void _slot_ReplaceAll();
	void _slot_Count();
	void _slot_FindAll();
	void _slot_FindInFiles();
	void _slot_ReplaceInFiles();
	void _slot_Browse();
	void _slot_FocusChanged(QWidget *old, QWidget *now);
	void _slot_CurrentChanged(int index);
	void _slot_FindReplaceTextChanged();
	void _slot_DirectoryTextChanged();
protected:
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);

	bool _FindFirst(bool skipSelection = true);
	bool _Replace();
	void _FindInFiles(bool replacing = false);
	bool _IsFindEnabled() const;
	bool _IsFindInFilesEnabled() const;
	bool _IsReplaceInFilesEnabled() const;
	bool _IsReplaceEnabled() const;
	int _FindHelper(const QString &filePath, bool emitting);

	class ComboBoxArea;
	class ButtonsArea;
	class OptionsArea;

	ComboBoxArea *comboboxArea;
	ButtonsArea *buttonsArea;
	OptionsArea *optionsArea;
	QTabBar * _tabbar;
	FScintilla *_editor;
	FScintilla *_hiddenEditor;
	QRect _geometry;
};

} // namespace fedup

#endif
