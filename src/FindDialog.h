#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QCheckBox;
class QRadioButton;
QT_END_NAMESPACE

namespace fedup {

class FindDialog : public QDialog
{
	Q_OBJECT
public:
	FindDialog(QWidget *parent = NULL);
	~FindDialog();
signals:
	void findFirst(const QString &expr, bool re, bool cs, bool wo, bool wrap, bool forward);
	// void find(const QString &expr, bool re, bool cs, bool wo, bool wrap, bool forward);
	void findNext();
	// void countOccurrences(const QString &needle);
protected slots:
	void slot_FindNext();
	// void slot_FindPrev();
	void slot_FocusChanged(QWidget *old, QWidget *now);
protected:
	QLineEdit *combobox;
	QCheckBox *wholeWord;
	QCheckBox *caseSensitive;
	QCheckBox *wrapAround;
	QRadioButton *regularExpressionMode;
	QRadioButton *downDirection;
};

} // namespace fedup

#endif
