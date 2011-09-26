#include "FindDialog.h"
#include "FScintilla.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStackedLayout>
#include <QTabBar>
#include <QFrame>
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
#include <QFileDialog>
#include <QDebug>
namespace fedup {

class FindDialog::ComboBoxArea
{
public:
	ComboBoxArea(QWidget *parent);

	QGridLayout * grid;
	QLabel * findLabel;
	QLabel * replaceLabel;
	QLabel * filtersLabel;
	QLabel * directoryLabel;
	QComboBox * findCombobox;
	QComboBox * replaceCombobox;
	QComboBox * filtersCombobox;
	QComboBox * directoryCombobox;
	QPushButton * browseButton;
};

FindDialog::ComboBoxArea::ComboBoxArea(QWidget *parent) :
	grid(NULL),
	findLabel(NULL),
	replaceLabel(NULL),
	filtersLabel(NULL),
	directoryLabel(NULL),
	findCombobox(NULL),
	replaceCombobox(NULL),
	filtersCombobox(NULL),
	directoryCombobox(NULL),
	browseButton(NULL)
{
	grid = new QGridLayout;

	findLabel = new QLabel("&Find what:", parent);
	grid->addWidget(findLabel, 0, 0, Qt::AlignRight);
	findCombobox = new QComboBox(parent);
	findCombobox->setEditable(true);
	findLabel->setBuddy(findCombobox);
	grid->addWidget(findCombobox, 0, 1, 1, 2);

	replaceLabel = new QLabel("Rep&lace with:", parent);
	grid->addWidget(replaceLabel, 1, 0, Qt::AlignRight);
	replaceCombobox = new QComboBox(parent);
	replaceCombobox->setEditable(true);
	replaceLabel->setBuddy(replaceCombobox);
	grid->addWidget(replaceCombobox, 1, 1, 1, 2);

	filtersLabel = new QLabel("Filter&s:", parent);
	grid->addWidget(filtersLabel, 2, 0, Qt::AlignRight);
	filtersCombobox = new QComboBox(parent);
	filtersCombobox->setEditable(true);
	filtersLabel->setBuddy(filtersCombobox);
	grid->addWidget(filtersCombobox, 2, 1, 1, 2);

	directoryLabel = new QLabel("Dir&ectory:", parent);
	grid->addWidget(directoryLabel, 3, 0, Qt::AlignRight);
	directoryCombobox = new QComboBox(parent);
	directoryCombobox->setEditable(true);
	directoryLabel->setBuddy(directoryCombobox);
	grid->addWidget(directoryCombobox, 3, 1);

	browseButton = new QPushButton("...", parent);
	browseButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	grid->addWidget(browseButton, 3, 2);

	grid->setColumnStretch(1, 1);
	grid->setRowStretch(4, 1);
}

class FindButtonsArea : public QWidget
{
public:
	FindButtonsArea(QWidget *parent) :
		QWidget(parent),
		findNextButton(NULL),
		countButton(NULL),
		findAllButton(NULL),
		findAllInOpenDocumentsButton(NULL),
		closeButton(NULL)
	{
		QVBoxLayout * const vbox = new QVBoxLayout(this);
		vbox->setSpacing(0);
		vbox->setContentsMargins(0, 0, 0, 0);
		findNextButton = new QPushButton("Find Next", this);
		countButton = new QPushButton("Count", this);
		findAllButton = new QPushButton("Find All", this);
		closeButton = new QPushButton("Close", this);
		findAllInOpenDocumentsButton = new QPushButton("Find All in All Opened\nDocuments", this);
		vbox->addWidget(findNextButton);
		vbox->addWidget(countButton);
		vbox->addWidget(findAllButton);
		vbox->addWidget(findAllInOpenDocumentsButton);
		vbox->addStretch();
		vbox->addWidget(closeButton);
	}
public:
	QPushButton * findNextButton;
	QPushButton * countButton;
	QPushButton * findAllButton;
	QPushButton * findAllInOpenDocumentsButton;
	QPushButton * closeButton;
};

class ReplaceButtonsArea : public QWidget
{
public:
	ReplaceButtonsArea(QWidget *parent) :
		QWidget(parent),
		findNextButton(NULL),
		replaceButton(NULL),
		replaceAllButton(NULL),
		replaceAllInOpenDocumentsButton(NULL),
		closeButton(NULL)
	{
		QVBoxLayout * const vbox = new QVBoxLayout(this);
		vbox->setSpacing(0);
		vbox->setContentsMargins(0, 0, 0, 0);
		findNextButton = new QPushButton("Find Next", this);
		replaceButton = new QPushButton("Replace", this);
		replaceAllButton = new QPushButton("Replace All", this);
		replaceAllInOpenDocumentsButton = new QPushButton("Replace All in All Opened\nDocuments", this);
		closeButton = new QPushButton("Close", this);
		vbox->addWidget(findNextButton);
		vbox->addWidget(replaceButton);
		vbox->addWidget(replaceAllButton);
		vbox->addWidget(replaceAllInOpenDocumentsButton);
		vbox->addStretch();
		vbox->addWidget(closeButton);
	}
public:
	QPushButton * findNextButton;
	QPushButton * replaceButton;
	QPushButton * replaceAllButton;
	QPushButton * replaceAllInOpenDocumentsButton;
	QPushButton * closeButton;
};

class MarkButtonsArea : public QWidget
{
public:
	MarkButtonsArea(QWidget *parent) : QWidget(parent)
	{
		QVBoxLayout * const vbox = new QVBoxLayout(this);
		vbox->setSpacing(0);
		vbox->setContentsMargins(0, 0, 0, 0);
		markAllButton = new QPushButton("Mark All", this);
		clearAllButton = new QPushButton("Clear All Marks", this);
		closeButton = new QPushButton("Close", this);
		vbox->addWidget(markAllButton);
		vbox->addWidget(clearAllButton);
		vbox->addStretch();
		vbox->addWidget(closeButton);
	}
public:
	QPushButton * markAllButton;
	QPushButton * clearAllButton;
	QPushButton * closeButton;
};

class FindInFilesButtonsArea : public QWidget
{
public:
	FindInFilesButtonsArea(QWidget *parent) : QWidget(parent)
	{
		QVBoxLayout * const vbox = new QVBoxLayout(this);
		vbox->setSpacing(0);
		vbox->setContentsMargins(0, 0, 0, 0);
		findAllButton = new QPushButton("Find All", this);
		replaceAllButton = new QPushButton("Replace in Files", this);
		closeButton = new QPushButton("Close", this);
		vbox->addWidget(findAllButton);
		vbox->addWidget(replaceAllButton);
		vbox->addWidget(new QCheckBox("Follow current doc.", this));
		vbox->addWidget(new QCheckBox("In all su&b-folders", this));
		vbox->addWidget(new QCheckBox("In &hidden folders", this));
		vbox->addStretch();
		vbox->addWidget(closeButton);
	}
public:
	QPushButton * findAllButton;
	QPushButton * replaceAllButton;
	QPushButton * closeButton;
};

class FindDialog::ButtonsArea
{
public:
	ButtonsArea(QWidget *parent) :
		stack(new QStackedLayout),
		find(new FindButtonsArea(parent)),
		replace(new ReplaceButtonsArea(parent)),
		mark(new MarkButtonsArea(parent)),
		findInFiles(new FindInFilesButtonsArea(parent))
	{
		/*stack->setSpacing(0);
		stack->setContentsMargins(0, 0, 0, 0);
		stack->setAlignment(Qt::AlignHCenter | Qt::AlignTop);*/

		stack->addWidget(find);
		stack->addWidget(replace);
		stack->addWidget(mark);
		stack->addWidget(findInFiles);
		stack->setCurrentIndex(2);
	}
public:
	QStackedLayout * stack;
	FindButtonsArea * find;
	ReplaceButtonsArea * replace;
	MarkButtonsArea * mark;
	FindInFilesButtonsArea * findInFiles;
};

class OptionsArea
{
public:
	OptionsArea(QWidget *parent) : hbox2(NULL)
	{
		hbox2 = new QHBoxLayout;
		{
			QGroupBox * const groupbox = new QGroupBox("Options", parent);
			QVBoxLayout * const vbox2 = new QVBoxLayout(groupbox);
			vbox2->setSpacing(0);
			// vbox2->setContentsMargins(0, 0, 0, 0);
			wholeWord = new QCheckBox("Match &whole word only", parent);
			vbox2->addWidget(wholeWord);
			caseSensitive = new QCheckBox("Match &case", parent);
			vbox2->addWidget(caseSensitive);
			wrapAround = new QCheckBox("Wra&p around", parent);
			vbox2->addWidget(wrapAround);
			hbox2->addWidget(groupbox);
		}
		{
			QGroupBox * const groupbox = new QGroupBox("Search Mode", parent);
			QVBoxLayout * const vbox2 = new QVBoxLayout(groupbox);
			normalMode = new QRadioButton("&Normal", parent);
			vbox2->setSpacing(0);
			// vbox2->setContentsMargins(0, 0, 0, 0);
			extendedMode = new QRadioButton("E&xtended (\\n, \\r, \\t, \\0, \\x...)", parent);
			regularExpressionMode = new QRadioButton("Regular e&xpression", parent);
			vbox2->addWidget(normalMode);
			vbox2->addWidget(extendedMode);
			vbox2->addWidget(regularExpressionMode);
			hbox2->addWidget(groupbox, 1);

			normalMode->setChecked(true);
		}
		{
			QGroupBox * const groupbox = new QGroupBox("Direction", parent);
			QVBoxLayout * const vbox2 = new QVBoxLayout(groupbox);
			vbox2->setSpacing(0);
			// vbox2->setContentsMargins(0, 0, 0, 0);
			vbox2->addWidget(upDirection = new QRadioButton("&Up", parent));
			vbox2->addWidget(downDirection = new QRadioButton("&Down", parent));
			hbox2->addWidget(groupbox);
			vbox2->addStretch();

			downDirection->setChecked(true);
		}
		{
			QGroupBox * const groupbox = new QGroupBox("Transparenc&y", parent);
			groupbox->setCheckable(true);
			QVBoxLayout * const vbox2 = new QVBoxLayout(groupbox);
			vbox2->setSpacing(0);
			// vbox2->setContentsMargins(0, 0, 0, 0);
			QRadioButton * const transparentOnLosingFocus = new QRadioButton("On losing focus", parent);
			QRadioButton * const transparentAlways = new QRadioButton("Always", parent);
			vbox2->addWidget(transparentOnLosingFocus);
			vbox2->addWidget(transparentAlways);
			vbox2->addWidget(new QSlider(Qt::Horizontal, parent));
			hbox2->addWidget(groupbox);

			transparentOnLosingFocus->setChecked(true);
		}
	}
public:
	QHBoxLayout * hbox2;
	QCheckBox * wholeWord;
	QCheckBox * caseSensitive;
	QCheckBox * wrapAround;
	QRadioButton * normalMode;
	QRadioButton * extendedMode;
	QRadioButton * regularExpressionMode;
	QRadioButton * upDirection;
	QRadioButton * downDirection;
};

static QString ConvertFromExtended(const QString &query);

FindDialog::FindDialog(FScintilla *editor, QWidget *parent) : QDialog(parent, Qt::Tool),
	comboboxArea(NULL), _tabbar(NULL), combobox(NULL),
	wholeWord(NULL), caseSensitive(NULL), wrapAround(NULL),
	extendedMode(NULL), regularExpressionMode(NULL),
	downDirection(NULL), _editor(editor)
{
	_tabbar = new QTabBar(this);
	{
		// tabbar->setDrawBase(false);
		_tabbar->setExpanding(false);
		_tabbar->addTab("Find");
		_tabbar->addTab("Replace");
		_tabbar->addTab("Mark");
		_tabbar->addTab("Find in Files");
	}
	QFrame * const frame = new QFrame(this);
	frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
	// TODO make this look right using similar methods to QTabWidget w/ QStyleOptionTabWidgetFrameV2 and whatnot

	comboboxArea = new ComboBoxArea(frame);
	OptionsArea optionsArea(frame);
	buttonsArea = new ButtonsArea(frame);

	QVBoxLayout * const vboxOuter = new QVBoxLayout(this);
	vboxOuter->setSpacing(0);
	vboxOuter->setContentsMargins(0, 0, 0, 0);
	vboxOuter->setSizeConstraint(QLayout::SetFixedSize);

	{
		QVBoxLayout * const vbox = new QVBoxLayout(frame);
		QHBoxLayout * const hbox = new QHBoxLayout;
			hbox->addLayout(comboboxArea->grid, 1);
			hbox->addLayout(buttonsArea->stack);
		vbox->addLayout(hbox);
		vbox->addLayout(optionsArea.hbox2);
	}
	vboxOuter->addWidget(_tabbar);
	vboxOuter->addWidget(frame);

	combobox = comboboxArea->findCombobox;
	wholeWord = optionsArea.wholeWord;
	caseSensitive = optionsArea.caseSensitive;
	wrapAround = optionsArea.wrapAround;
	extendedMode = optionsArea.extendedMode;
	regularExpressionMode = optionsArea.regularExpressionMode;
	downDirection = optionsArea.downDirection;

	connect(_tabbar, SIGNAL(currentChanged(int)), buttonsArea->stack, SLOT(setCurrentIndex(int)));
	connect(_tabbar, SIGNAL(currentChanged(int)), this, SLOT(_slot_CurrentChanged(int)));
	connect(buttonsArea->find->closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(buttonsArea->replace->closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(buttonsArea->findInFiles->closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(buttonsArea->mark->closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(buttonsArea->find->findNextButton, SIGNAL(clicked()), this, SLOT(_slot_FindNext()));
	connect(buttonsArea->replace->findNextButton, SIGNAL(clicked()), this, SLOT(_slot_FindNext()));
	connect(buttonsArea->replace->replaceButton, SIGNAL(clicked()), this, SLOT(_slot_Replace()));
	connect(buttonsArea->replace->replaceAllButton, SIGNAL(clicked()), this, SLOT(_slot_ReplaceAll()));
	connect(combobox->lineEdit(), SIGNAL(returnPressed()), buttonsArea->find->findNextButton, SIGNAL(clicked()));
	connect(comboboxArea->findCombobox->lineEdit(), SIGNAL(textChanged(const QString &)), this, SLOT(_slot_FindReplaceTextChanged()));
	connect(comboboxArea->replaceCombobox->lineEdit(), SIGNAL(textChanged(const QString &)), this, SLOT(_slot_FindReplaceTextChanged()));
	connect(comboboxArea->browseButton, SIGNAL(clicked()), this, SLOT(_slot_Browse()));
	connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)), this, SLOT(_slot_FocusChanged(QWidget *, QWidget *)));

	// to initialize it to the right state we call these slots as if the signals were triggered
	_slot_CurrentChanged(_tabbar->currentIndex());
	_slot_FindReplaceTextChanged();
}

FindDialog::~FindDialog()
{
	delete buttonsArea;
}

void FindDialog::showFind()
{
	_tabbar->setCurrentIndex(0); // TODO use an enum rather than magic numbers
	show();
	raise();
	activateWindow();
}

void FindDialog::showReplace()
{
	_tabbar->setCurrentIndex(1); // TODO use an enum rather than magic numbers
	show();
	raise();
	activateWindow();
}

bool FindDialog::_FindFirst(bool skipSelection)
{
	if (combobox->currentText().size() == 0)
		return false;

	int lineFrom = -1, indexFrom = -1, lineTo = -1, indexTo = -1;
	_editor->getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
	// TODO use a boolean xor rather than bitwise?
	int line  = (downDirection->isChecked() ^ !skipSelection) ? lineTo  : lineFrom;
	int index = (downDirection->isChecked() ^ !skipSelection) ? indexTo : indexFrom;
	return _editor->findFirst(extendedMode->isChecked() ? ConvertFromExtended(combobox->currentText()) : combobox->currentText(), regularExpressionMode->isChecked(), caseSensitive->isChecked(), wholeWord->isChecked(), wrapAround->isChecked(), downDirection->isChecked(), line, index);
}

bool FindDialog::_Replace()
{
	int lineFrom = -1, indexFrom = -1, lineTo = -1, indexTo = -1;
	_editor->getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
	if (!_FindFirst(false))
		return false;
	{
		int lineFrom2 = -1, indexFrom2 = -1, lineTo2 = -1, indexTo2 = -1;
		_editor->getSelection(&lineFrom2, &indexFrom2, &lineTo2, &indexTo2);
		if (lineFrom == lineFrom2 && indexFrom == indexFrom2 && lineTo == lineTo2 && indexTo == indexTo2)
		{
			qDebug() << "replaced line" << lineFrom;
			_editor->replace(comboboxArea->replaceCombobox->currentText());
			if (!_FindFirst(true))
				_editor->setCursorPosition(lineTo, indexTo); // HACK to avoid an infinite loop
		}
	}
	return true;
}

void FindDialog::_slot_FindNext()
{
	_FindFirst();
}

void FindDialog::_slot_Replace()
{
	_Replace();
}

void FindDialog::_slot_ReplaceAll()
{
	bool wrapAroundWasChecked = wrapAround->isChecked();
	wrapAround->setChecked(false); // HACK, should be totally safe though

	_editor->beginUndoAction();
	while (_Replace())
	{
		/* NO-OP */
	}
	_editor->endUndoAction();

	wrapAround->setChecked(wrapAroundWasChecked);
}

void FindDialog::_slot_Browse()
{
	// TODO
	const QString dirPath = QFileDialog::getExistingDirectory(this);
	if (dirPath.size() != 0)
		comboboxArea->directoryCombobox->lineEdit()->setText(dirPath);
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

void FindDialog::_slot_CurrentChanged(int index)
{
	// TODO use an enum rather than constants here
	const bool replaceSelected = (index == 1);
	const bool findInFilesSelected = (index == 3);

	comboboxArea->replaceLabel->setEnabled(replaceSelected || findInFilesSelected);
	comboboxArea->replaceCombobox->setEnabled(replaceSelected || findInFilesSelected);

	comboboxArea->filtersLabel->setEnabled(findInFilesSelected);
	comboboxArea->filtersCombobox->setEnabled(findInFilesSelected);

	comboboxArea->directoryLabel->setEnabled(findInFilesSelected);
	comboboxArea->directoryCombobox->setEnabled(findInFilesSelected);
	comboboxArea->browseButton->setEnabled(findInFilesSelected);
}

void FindDialog::_slot_FindReplaceTextChanged()
{
	const bool findEnabled = (comboboxArea->findCombobox->currentText().size() != 0);
	buttonsArea->find->findNextButton->setEnabled(findEnabled);
	buttonsArea->find->countButton->setEnabled(findEnabled);
	buttonsArea->find->findAllButton->setEnabled(findEnabled);
	buttonsArea->find->findAllInOpenDocumentsButton->setEnabled(findEnabled);
	buttonsArea->replace->findNextButton->setEnabled(findEnabled);
	buttonsArea->mark->markAllButton->setEnabled(findEnabled);
	buttonsArea->findInFiles->findAllButton->setEnabled(findEnabled);

	const bool replaceEnabled = (comboboxArea->replaceCombobox->currentText().size() != 0) && findEnabled;
	buttonsArea->replace->replaceButton->setEnabled(replaceEnabled);
	buttonsArea->replace->replaceAllButton->setEnabled(replaceEnabled);
	buttonsArea->replace->replaceAllInOpenDocumentsButton->setEnabled(replaceEnabled);
	buttonsArea->findInFiles->replaceAllButton->setEnabled(replaceEnabled);
}

void FindDialog::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);
	if (!event->spontaneous())
	{
		// TODO set the directoryCombobox text based on the current document, if that behavior is enabled
		const QString selectedText = _editor->selectedText();
		if (selectedText.size() != 0)
			combobox->lineEdit()->setText(selectedText);
		if (!_geometry.isNull())
			setGeometry(_geometry);
		combobox->setFocus();
	}
}

void FindDialog::hideEvent(QHideEvent *event)
{
	if (!event->spontaneous())
		_geometry = geometry();
}

static QString ConvertFromExtended(const QString &query)
{
	QString result;
	result.reserve(query.size());
	for (int i = 0; i < query.size(); /* NO-OP */)
	{
		QChar current = query.at(i++);
		if (current == '\\' && i != query.size())
		{
			current = query.at(i++);
			switch (current.toAscii())
			{
				case 'r': result.append('\r'); break;
				case 'n': result.append('\n'); break;
				case '0': result.append('\0'); break;
				case 't': result.append('\t'); break;
				case '\\': result.append('\\'); break;
				case 'b':
				case 'd':
				case 'o':
				case 'x':
				case 'u':
				{
					int size = 0, base = 0;
					if (current == 'b') { //11111111
						size = 8, base = 2;
					} else if (current == 'o') { //377
						size = 3, base = 8;
					} else if (current == 'd') { //255
						size = 3, base = 10;
					} else if (current == 'x') { //0xFF
						size = 2, base = 16;
					} else if (current == 'u') { //0xCDCD
						size = 4, base = 16;
					}
					if (i + size <= query.size())
					{
						bool ok = false;
						const ushort value = query.mid(i, size).toUShort(&ok, base);
						if (ok)
						{
							if (current == 'u')
								result.append(QChar(value));
							else
								result.append(QChar(static_cast<char>(value)));
							i += size;
							break;
						}
						// otherwise we couldn't convert it successfully, fall back to normal output
					}
					//not enough chars to make parameter, use default method as fallback
				}
				default:
				{
					result.append('\\');
					result.append(current);
					break;
				}
			}
		}
		else
		{
			result.append(current);
		}
	}
	return result;
}

} // namespace fedup
