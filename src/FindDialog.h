#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QRect>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class QRadioButton;
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
	void _slot_FocusChanged(QWidget *old, QWidget *now);
protected:
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);

	QComboBox *combobox;
	QCheckBox *wholeWord;
	QCheckBox *caseSensitive;
	QCheckBox *wrapAround;
	QRadioButton *regularExpressionMode;
	QRadioButton *downDirection;
	FScintilla *_editor;
	QRect _geometry;
};

} // namespace fedup

#endif
