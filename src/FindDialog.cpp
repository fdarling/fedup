#include "FindDialog.h"
#include "FScintilla.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
// #include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QSlider>
#include <QShowEvent>
#include <QHideEvent>
#include <QLineEdit>

#include <QApplication>

namespace fedup {

FindDialog::FindDialog(FScintilla *editor, QWidget *parent) : QDialog(parent, Qt::Tool),
	combobox(NULL),
	wholeWord(NULL), caseSensitive(NULL), wrapAround(NULL),
	regularExpressionMode(NULL),
	downDirection(NULL), _editor(editor)
{
	// setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	// setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QHBoxLayout * const hbox = new QHBoxLayout(this);
	hbox->setSizeConstraint(QLayout::SetFixedSize);
	{
		QVBoxLayout * const vbox = new QVBoxLayout;
		{
			QHBoxLayout * const hbox2 = new QHBoxLayout;
			QLabel * const findLabel = new QLabel("&Find what:", this);
			hbox2->addWidget(findLabel);
			combobox = new QComboBox(this);
			combobox->setEditable(true);
			findLabel->setBuddy(combobox);
			hbox2->addWidget(combobox, 1);
			vbox->addLayout(hbox2);
		}
		{
			QGroupBox * const groupbox = new QGroupBox(this);
			QHBoxLayout * const hbox2 = new QHBoxLayout(groupbox);
			{
				QVBoxLayout * const vbox2 = new QVBoxLayout;
				vbox2->addWidget(new QCheckBox("&Mark line", this));
				vbox2->addWidget(new QCheckBox("Style found to&ken", this));
				vbox2->addWidget(new QCheckBox("Purge for each search", this));
				hbox2->addLayout(vbox2);
			}
			{
				QVBoxLayout * const vbox2 = new QVBoxLayout;
				vbox2->addWidget(new QPushButton("Find &All", this));
				vbox2->addWidget(new QCheckBox("In selection", this));
				vbox2->addWidget(new QPushButton("Clear", this));
				hbox2->addLayout(vbox2);
			}
			vbox->addWidget(groupbox);
		}
		{
			QVBoxLayout * const vbox2 = new QVBoxLayout;
			wholeWord = new QCheckBox("Match &whole word only", this);
			vbox2->addWidget(wholeWord);
			caseSensitive = new QCheckBox("Match &case", this);
			vbox2->addWidget(caseSensitive);
			wrapAround = new QCheckBox("Wra&p around", this);
			vbox2->addWidget(wrapAround);
			vbox->addLayout(vbox2);
		}
		{
			QHBoxLayout * const hbox2 = new QHBoxLayout;
			{
				QGroupBox * const groupbox = new QGroupBox("Search Mode", this);
				QVBoxLayout * const vbox2 = new QVBoxLayout(groupbox);
				QRadioButton * const normalMode = new QRadioButton("&Normal", this);
				vbox2->addWidget(normalMode);
				vbox2->addWidget(new QRadioButton("E&xtended (\\n, \\r, \\t, \\0, \\x...)", this));
				regularExpressionMode = new QRadioButton("Regular e&xpression", this);
				vbox2->addWidget(regularExpressionMode);
				hbox2->addWidget(groupbox, 1);

				normalMode->setChecked(true);
			}
			{
				QGroupBox * const groupbox = new QGroupBox("Direction", this);
				QVBoxLayout * const vbox2 = new QVBoxLayout(groupbox);
				vbox2->addWidget(new QRadioButton("&Up", this));
				downDirection = new QRadioButton("&Down", this);
				vbox2->addWidget(downDirection);
				hbox2->addWidget(groupbox);

				downDirection->setChecked(true);
			}
			vbox->addLayout(hbox2);

		}
		hbox->addLayout(vbox, 1);
	}
	{
		QVBoxLayout * const vbox = new QVBoxLayout;
		{
			QPushButton * const findNextButton = new QPushButton("Find Next", this);
			// connect(combobox, SIGNAL(returnPressed()), findNextButton, SIGNAL(clicked())); // TODO detect return being pressed manually!
			// QPushButton * const findPrevButton = new QPushButton("Find Prev", this);
			connect(findNextButton, SIGNAL(clicked()), this, SLOT(_slot_FindNext()));
			// connect(findPrevButton, SIGNAL(clicked()), this, SLOT(slot_FindPrev()));
			vbox->addWidget(findNextButton);
			// vbox->addWidget(findPrevButton);
			vbox->addWidget(new QPushButton("Count", this));
			vbox->addWidget(new QPushButton("Find All in All Opened\nDocuments", this));
			vbox->addWidget(new QPushButton("Find All in Current\nDocument", this));
			QPushButton * const closeButton = new QPushButton("Close", this);
			connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
			vbox->addWidget(closeButton);
			hbox->addLayout(vbox);
		}
		vbox->addStretch();
		{
			QGroupBox * const groupbox = new QGroupBox("Transparenc&y", this);
			groupbox->setCheckable(true);
			QVBoxLayout * const vbox2 = new QVBoxLayout(groupbox);
			// vbox2->setContentsMargins(0, 0, 0, 0);
			QRadioButton * const transparentOnLosingFocus = new QRadioButton("On losing focus", this);
			QRadioButton * const transparentAlways = new QRadioButton("Always", this);
			vbox2->addWidget(transparentOnLosingFocus);
			vbox2->addWidget(transparentAlways);
			vbox2->addWidget(new QSlider(Qt::Horizontal, this));
			vbox->addWidget(groupbox);

			transparentOnLosingFocus->setChecked(true);
		}
	}

	// setFocusPolicy(Qt::StrongFocus);
	connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)), this, SLOT(_slot_FocusChanged(QWidget *, QWidget *)));

	combobox->setFocus();
}

FindDialog::~FindDialog()
{
}

void FindDialog::_slot_FindNext()
{
	if (combobox->currentText().size() == 0)
		return;

	int lineFrom = -1, indexFrom = -1, lineTo = -1, indexTo = -1;
	_editor->getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
	int line  = downDirection->isChecked() ? lineTo  : lineFrom;
	int index = downDirection->isChecked() ? indexTo : indexFrom;
	// TODO implement the "Extended" mode
	_editor->findFirst(combobox->currentText(), regularExpressionMode->isChecked(), caseSensitive->isChecked(), wholeWord->isChecked(), wrapAround->isChecked(), downDirection->isChecked(), line, index);
}

void FindDialog::_slot_FocusChanged(QWidget *old, QWidget *now)
{
	Q_UNUSED(old);
	Q_UNUSED(now);
	// HACK, I use 0.99 instead of 1.0 to force transparency to be enabled, that way it won't glitch
	// when the transparency is lowered as it would if transparency were enabled, *then* lowered
	// TODO make this actually respect the settings!
	setWindowOpacity(isActiveWindow() ? 0.99 : 0.5);
}

void FindDialog::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);
	if (!event->spontaneous())
	{
		combobox->lineEdit()->setText(_editor->selectedText());
		if (!_geometry.isNull())
			setGeometry(_geometry);
	}
}

void FindDialog::hideEvent(QHideEvent *event)
{
	if (!event->spontaneous())
		_geometry = geometry();
}

} // namespace fedup
