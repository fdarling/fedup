#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QRect>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class QRadioButton;
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
protected slots:
	void _slot_FindNext();
	void _slot_Replace();
	void _slot_ReplaceAll();
	void _slot_FocusChanged(QWidget *old, QWidget *now);
	void _slot_Browse();
	void _slot_CurrentChanged(int index);
	void _slot_FindReplaceTextChanged();
protected:
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);

	bool _FindFirst(bool skipSelection = true);
	bool _Replace();

	class ComboBoxArea;
	class ButtonsArea;

	ComboBoxArea *comboboxArea;
	ButtonsArea *buttonsArea;
	QComboBox *combobox;
	QCheckBox *wholeWord;
	QCheckBox *caseSensitive;
	QCheckBox *wrapAround;
	QRadioButton *extendedMode;
	QRadioButton *regularExpressionMode;
	QRadioButton *downDirection;
	FScintilla *_editor;
	QRect _geometry;
};

} // namespace fedup

#endif
