#include "GoToDialog.h"
#include <QLineEdit>
#include <QIntValidator>
#include <QRadioButton>
#include <QLabel>

#include <QGridLayout>
#include <QRadioButton>
#include <QPushButton>

namespace fedup {

GoToDialog::GoToDialog(QWidget *parent) : QDialog(parent, Qt::Tool), lineedit(NULL), lineValidator(NULL), offsetValidator(NULL), lineMode(NULL), currentLabel(NULL), maxLabel(NULL), currentLine(1), currentOffset(0)
{
	lineValidator = new QIntValidator(1, 1, this);
	offsetValidator = new QIntValidator(0, 0, this);

	QGridLayout * const grid = new QGridLayout(this);
	grid->setSizeConstraint(QLayout::SetFixedSize);
	lineMode = new QRadioButton("&Line", this);
	connect(lineMode, SIGNAL(toggled(bool)), this, SLOT(slot_LineMode(bool)));
	grid->addWidget(lineMode, 0, 0);
	grid->addWidget(new QRadioButton("&Offset"), 0, 1);
	grid->addWidget(new QLabel("You are here:"), 1, 0);
	currentLabel = new QLabel("0", this);
	grid->addWidget(currentLabel, 1, 1);
	grid->addWidget(new QLabel("You want to go to:"), 2, 0);
	lineedit = new QLineEdit(this);
	grid->addWidget(lineedit, 2, 1);
	QPushButton * const goButton = new QPushButton("Go");
	connect(lineedit, SIGNAL(returnPressed()), goButton, SIGNAL(clicked()));
	connect(goButton, SIGNAL(clicked()), this, SLOT(slot_Go()));
	grid->addWidget(goButton, 2, 2);
	grid->addWidget(new QLabel("You can't go further than:"), 3, 0);
	maxLabel = new QLabel("1", this);
	grid->addWidget(maxLabel, 3, 1);
	QPushButton * const nowhereButton = new QPushButton("I'm going nowhere");
	connect(nowhereButton, SIGNAL(clicked()), this, SLOT(reject()));
	grid->addWidget(nowhereButton, 3, 2);

	lineMode->setChecked(true);
	lineedit->setValidator(lineValidator);
	lineedit->setFocus();
}

GoToDialog::~GoToDialog()
{
}

void GoToDialog::slot_SetCurrentLine(int line)
{
	currentLine = line + 1;
	if (lineMode->isChecked())
		currentLabel->setText(QString::number(currentLine));
}

void GoToDialog::slot_SetCurrentOffset(int offset)
{
	currentOffset = offset;
	if (!lineMode->isChecked())
		currentLabel->setText(QString::number(currentOffset));
}

void GoToDialog::slot_SetMaxLine(int line)
{
	lineValidator->setTop(line);
	if (lineMode->isChecked())
		maxLabel->setText(QString::number(line));
}

void GoToDialog::slot_SetMaxOffset(int offset)
{
	offsetValidator->setTop(offset);
	if (!lineMode->isChecked())
		maxLabel->setText(QString::number(offset));
}

void GoToDialog::slot_SetLength(int length)
{
	slot_SetMaxOffset(length-1);
}

void GoToDialog::slot_Go()
{
	// TODO possibly give an error if there is unacceptable input
	bool ok = false;
	const int number = lineedit->text().toInt(&ok);
	if (/*lineedit->hasAcceptableInput() && */ok)
	{
		if (lineMode->isChecked())
			emit goToLine(number-1);
		else
			emit goToOffset(number);
	}
	accept();
}

void GoToDialog::slot_LineMode(bool lineMode)
{
	QIntValidator * const validator = lineMode ? lineValidator : offsetValidator;
	lineedit->setValidator(validator);
	currentLabel->setText(QString::number(lineMode ? currentLine : currentOffset));
	maxLabel->setText(QString::number(validator->top()));
}

} // namespace fedup
