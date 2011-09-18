#ifndef GOTODIALOG_H
#define GOTODIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QIntValidator;
class QRadioButton;
class QLabel;
QT_END_NAMESPACE

namespace fedup {

class GoToDialog : public QDialog
{
	Q_OBJECT
public:
	GoToDialog(QWidget *parent = NULL);
	~GoToDialog();
signals:
	void goToLine(int line);
	void goToOffset(int offset);
public slots:
	void slot_SetCurrentLine(int line);
	void slot_SetCurrentOffset(int offset);
	void slot_SetMaxLine(int line);
	void slot_SetMaxOffset(int offset);
	void slot_SetLength(int length);
protected slots:
	void slot_Go();
	void slot_LineMode(bool lineMode);
protected:
	QLineEdit *lineedit;
	QIntValidator *lineValidator;
	QIntValidator *offsetValidator;
	QRadioButton *lineMode;
	QLabel *currentLabel;
	QLabel *maxLabel;
	int currentLine;
	int currentOffset;
};

} // namespace fedup

#endif
