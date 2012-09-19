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
#include <QDirIterator>
#include <QScopedPointer>
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

	filtersCombobox->lineEdit()->setText("*.*");
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
	FindInFilesButtonsArea(QWidget *parent) :
		QWidget(parent),
		findAllButton(NULL),
		replaceAllButton(NULL),
		followCurrentDocCheckbox(NULL),
		subfoldersCheckbox(NULL),
		hiddenFoldersCheckbox(NULL),
		closeButton(NULL)
	{
		QVBoxLayout * const vbox = new QVBoxLayout(this);
		vbox->setSpacing(0);
		vbox->setContentsMargins(0, 0, 0, 0);
		findAllButton = new QPushButton("Find All", this);
		replaceAllButton = new QPushButton("Replace in Files", this);
		followCurrentDocCheckbox = new QCheckBox("Follow current doc.", this);
		subfoldersCheckbox = new QCheckBox("In all su&b-folders", this);
		hiddenFoldersCheckbox = new QCheckBox("In &hidden folders", this);
		closeButton = new QPushButton("Close", this);
		vbox->addWidget(findAllButton);
		vbox->addWidget(replaceAllButton);
		vbox->addWidget(followCurrentDocCheckbox);
		vbox->addWidget(subfoldersCheckbox);
		vbox->addWidget(hiddenFoldersCheckbox);
		vbox->addStretch();
		vbox->addWidget(closeButton);

		subfoldersCheckbox->setChecked(true);
	}
public:
	QPushButton * findAllButton;
	QPushButton * replaceAllButton;
	QCheckBox * followCurrentDocCheckbox;
	QCheckBox * subfoldersCheckbox;
	QCheckBox * hiddenFoldersCheckbox;
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
	}
public:
	QStackedLayout * stack;
	FindButtonsArea * find;
	ReplaceButtonsArea * replace;
	MarkButtonsArea * mark;
	FindInFilesButtonsArea * findInFiles;
};

class FindDialog::OptionsArea
{
public:
	OptionsArea(QWidget *parent) :
		hbox2(NULL),
		wholeWord(NULL),
		caseSensitive(NULL),
		wrapAround(NULL),
		normalMode(NULL),
		extendedMode(NULL),
		regularExpressionMode(NULL),
		upDirection(NULL),
		downDirection(NULL)
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
			regularExpressionMode = new QRadioButton("Re&gular expression", parent);
			vbox2->addWidget(normalMode);
			vbox2->addWidget(extendedMode);
			vbox2->addWidget(regularExpressionMode);
			hbox2->addWidget(groupbox, 1);

			normalMode->setChecked(true);
		}
		{
			directionBox = new QGroupBox("Direction", parent);
			QVBoxLayout * const vbox2 = new QVBoxLayout(directionBox);
			vbox2->setSpacing(0);
			// vbox2->setContentsMargins(0, 0, 0, 0);
			vbox2->addWidget(upDirection = new QRadioButton("&Up", parent));
			vbox2->addWidget(downDirection = new QRadioButton("&Down", parent));
			hbox2->addWidget(directionBox);
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
	QGroupBox * directionBox;
	QRadioButton * upDirection;
	QRadioButton * downDirection;
};

static QString ConvertFromExtended(const QString &query);

FindDialog::FindDialog(FScintilla *editor, QWidget *parent) : QDialog(parent, Qt::Tool),
	comboboxArea(NULL), optionsArea(NULL), _tabbar(NULL),
	_editor(editor), _hiddenEditor(NULL)
{
	_tabbar = new QTabBar(this);
	{
		_tabbar->setExpanding(false);
		_tabbar->addTab("Find");
		_tabbar->addTab("Replace");
		_tabbar->addTab("Mark");
		_tabbar->addTab("Find in Files");
	}
	QFrame * const frame = new QFrame(this);
	frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
	// frame->setStyleSheet("border-top-style: none;")
	{
		QMargins margins = frame->contentsMargins();
		margins.setTop(0);
		frame->setContentsMargins(margins);
	}
	// TODO make this look right using similar methods to QTabWidget w/ QStyleOptionTabWidgetFrameV2 and whatnot

	comboboxArea = new ComboBoxArea(frame);
	optionsArea = new OptionsArea(frame);
	buttonsArea = new ButtonsArea(frame);
	_hiddenEditor = new FScintilla;

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
		vbox->addLayout(optionsArea->hbox2);
	}
	vboxOuter->addWidget(_tabbar);
	vboxOuter->addWidget(frame);

	connect(_tabbar, SIGNAL(currentChanged(int)), buttonsArea->stack, SLOT(setCurrentIndex(int)));
	connect(_tabbar, SIGNAL(currentChanged(int)), this, SLOT(_slot_CurrentChanged(int)));
	connect(buttonsArea->find->closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(buttonsArea->replace->closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(buttonsArea->findInFiles->findAllButton, SIGNAL(clicked()), this, SLOT(_slot_FindInFiles()));
	connect(buttonsArea->findInFiles->closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(buttonsArea->mark->closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(buttonsArea->find->findNextButton, SIGNAL(clicked()), this, SLOT(_slot_FindNext()));
	connect(buttonsArea->replace->findNextButton, SIGNAL(clicked()), this, SLOT(_slot_FindNext()));
	connect(buttonsArea->replace->replaceButton, SIGNAL(clicked()), this, SLOT(_slot_Replace()));
	connect(buttonsArea->replace->replaceAllButton, SIGNAL(clicked()), this, SLOT(_slot_ReplaceAll()));
	connect(comboboxArea->findCombobox->lineEdit(), SIGNAL(returnPressed()), buttonsArea->find->findNextButton, SIGNAL(clicked()));
	connect(comboboxArea->findCombobox->lineEdit(), SIGNAL(textChanged(const QString &)), this, SLOT(_slot_FindReplaceTextChanged()));
	connect(comboboxArea->directoryCombobox->lineEdit(), SIGNAL(textChanged(const QString &)), this, SLOT(_slot_DirectoryTextChanged()));
	connect(comboboxArea->replaceCombobox->lineEdit(), SIGNAL(textChanged(const QString &)), this, SLOT(_slot_FindReplaceTextChanged()));
	connect(comboboxArea->browseButton, SIGNAL(clicked()), this, SLOT(_slot_Browse()));
	connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)), this, SLOT(_slot_FocusChanged(QWidget *, QWidget *)));

	// to initialize it to the right state we call these slots as if the signals were triggered
	_slot_CurrentChanged(_tabbar->currentIndex());
	_slot_FindReplaceTextChanged();
	_slot_DirectoryTextChanged();
}

FindDialog::~FindDialog()
{
	delete buttonsArea;
	delete _hiddenEditor;
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

void FindDialog::showMark()
{
	_tabbar->setCurrentIndex(2); // TODO use an enum rather than magic numbers
	show();
	raise();
	activateWindow();
}

void FindDialog::showFindInFiles()
{
	_tabbar->setCurrentIndex(3); // TODO use an enum rather than magic numbers
	show();
	raise();
	activateWindow();
}

void FindDialog::findNext()
{
	const bool alreadyChecked = optionsArea->downDirection->isChecked();
	// (alreadyChecked ? _slot_FindNext : _slot_FindPrev)();
	if (!alreadyChecked)
		optionsArea->downDirection->setChecked(true); // HACK
	_slot_FindNext();
	if (!alreadyChecked)
		optionsArea->upDirection->setChecked(true); // HACK
}

void FindDialog::findPrev()
{
	const bool alreadyChecked = optionsArea->upDirection->isChecked();
	// (alreadyChecked ? _slot_FindNext : _slot_FindPrev)();
	if (!alreadyChecked)
		optionsArea->upDirection->setChecked(true); // HACK
	_slot_FindNext();
	if (!alreadyChecked)
		optionsArea->downDirection->setChecked(true); // HACK
}

bool FindDialog::_FindFirst(bool skipSelection)
{
	if (comboboxArea->findCombobox->currentText().size() == 0)
		return false;

	int lineFrom = -1, indexFrom = -1, lineTo = -1, indexTo = -1;
	_editor->getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
	// TODO use a boolean xor rather than bitwise?
	int line  = (optionsArea->downDirection->isChecked() ^ !skipSelection) ? lineTo  : lineFrom;
	int index = (optionsArea->downDirection->isChecked() ^ !skipSelection) ? indexTo : indexFrom;
	return _editor->findFirst(optionsArea->extendedMode->isChecked() ? ConvertFromExtended(comboboxArea->findCombobox->currentText()) : comboboxArea->findCombobox->currentText(), optionsArea->regularExpressionMode->isChecked(), optionsArea->caseSensitive->isChecked(), optionsArea->wholeWord->isChecked(), optionsArea->wrapAround->isChecked(), optionsArea->downDirection->isChecked(), line, index);
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
			_editor->replace(comboboxArea->replaceCombobox->currentText());
			if (!_FindFirst(true))
				_editor->setCursorPosition(lineTo, indexTo); // HACK to avoid an infinite loop
		}
	}
	return true;
}

void FindDialog::_FindInFiles(bool replacing)
{
	qDebug() << "_FindInFiles():";
	QDir::Filters dir_filters = QDir::Files | QDir::Readable;
	if (replacing)
		dir_filters |= QDir::Writable;
	if (buttonsArea->findInFiles->hiddenFoldersCheckbox->isChecked()) // TODO change this to a hiddenCheckbox and drop the folders specification?
		dir_filters |= QDir::Hidden;
	QDirIterator::IteratorFlags iterator_flags = QDirIterator::FollowSymlinks;
	if (buttonsArea->findInFiles->subfoldersCheckbox->isChecked())
		iterator_flags |= QDirIterator::Subdirectories;
	const QStringList filters = comboboxArea->filtersCombobox->currentText().split(QRegExp("\\s+"));
	bool useFilters = (filters.size() != 0);
	for (QStringList::const_iterator it = filters.begin(); it != filters.end(); ++it)
	{
		if (*it == "*" || *it == "*.*")
		{
			useFilters = false;
			break;
		}
	}
	QDirIterator it(comboboxArea->directoryCombobox->currentText(), useFilters ? filters : QStringList(), dir_filters, iterator_flags);
	while (it.hasNext())
	{
		const QString filePath = it.next();
		qDebug() << "\tSearching through file:" << filePath;
		_hiddenEditor->setDocument(QsciDocument());
		QFile file(filePath);

		if (file.open(QIODevice::ReadOnly) && _hiddenEditor->read(&file))
		{
			for (bool foundSomething = _hiddenEditor->findFirst(optionsArea->extendedMode->isChecked() ? ConvertFromExtended(comboboxArea->findCombobox->currentText()) : comboboxArea->findCombobox->currentText(), optionsArea->regularExpressionMode->isChecked(), optionsArea->caseSensitive->isChecked(), optionsArea->wholeWord->isChecked(), false, true, -1, -1, false); foundSomething; foundSomething = _hiddenEditor->findNext())
			{
				int lineFrom = -1, indexFrom = -1, lineTo = -1, indexTo = -1;
				_hiddenEditor->getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
				if (lineFrom == lineTo)
					qDebug() << "\t\tLine " << (lineFrom + 1) << ":" << _hiddenEditor->selectedText();
				else
					qDebug() << "\t\tLines" << (lineFrom + 1) << "-" << (lineTo + 1) << ":" << _hiddenEditor->selectedText();
			}
		}
		_hiddenEditor->setDocument(QsciDocument());
	}
	qDebug () << "Done crawling directories";
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
	bool wrapAroundWasChecked = optionsArea->wrapAround->isChecked();
	optionsArea->wrapAround->setChecked(false); // HACK, should be totally safe though

	_editor->beginUndoAction();
	while (_Replace())
	{
		/* NO-OP */
	}
	_editor->endUndoAction();

	optionsArea->wrapAround->setChecked(wrapAroundWasChecked);
}

void FindDialog::_slot_FindInFiles()
{
	_FindInFiles(false);
}

void FindDialog::_slot_ReplaceInFiles()
{
	_FindInFiles(true);
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

	optionsArea->directionBox->setEnabled(!findInFilesSelected);
	optionsArea->wrapAround->setEnabled(!findInFilesSelected);

	setWindowTitle(_tabbar->tabText(index));
}

bool FindDialog::_IsFindEnabled() const
{
	return (comboboxArea->findCombobox->currentText().size() != 0);
}

bool FindDialog::_IsFindInFilesEnabled() const
{
	return _IsFindEnabled() && (comboboxArea->directoryCombobox->currentText().size() != 0);
}

bool FindDialog::_IsReplaceEnabled() const
{
	// NOTE: replacing with nothing is allowed!
	return /*(comboboxArea->replaceCombobox->currentText().size() != 0) && */_IsFindEnabled();
}

bool FindDialog::_IsReplaceInFilesEnabled() const
{
	return _IsReplaceEnabled() && (comboboxArea->directoryCombobox->currentText().size() != 0);
}

void FindDialog::_slot_FindReplaceTextChanged()
{
	const bool findEnabled = _IsFindEnabled();
	const bool replaceEnabled = _IsReplaceEnabled();

	buttonsArea->find->findNextButton->setEnabled(findEnabled);
	buttonsArea->find->countButton->setEnabled(findEnabled);
	buttonsArea->find->findAllButton->setEnabled(findEnabled);
	buttonsArea->find->findAllInOpenDocumentsButton->setEnabled(findEnabled);
	buttonsArea->replace->findNextButton->setEnabled(findEnabled);
	buttonsArea->mark->markAllButton->setEnabled(findEnabled);
	buttonsArea->findInFiles->findAllButton->setEnabled(_IsFindInFilesEnabled());

	buttonsArea->replace->replaceButton->setEnabled(replaceEnabled);
	buttonsArea->replace->replaceAllButton->setEnabled(replaceEnabled);
	buttonsArea->replace->replaceAllInOpenDocumentsButton->setEnabled(replaceEnabled);
	buttonsArea->findInFiles->replaceAllButton->setEnabled(_IsReplaceInFilesEnabled());
}

void FindDialog::_slot_DirectoryTextChanged()
{
	_slot_FindReplaceTextChanged();
}

void FindDialog::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);
	if (!event->spontaneous())
	{
		// TODO set the directoryCombobox text based on the current document, if that behavior is enabled
		const QString selectedText = _editor->selectedText();
		if (selectedText.size() != 0)
			comboboxArea->findCombobox->lineEdit()->setText(selectedText);
		if (!_geometry.isNull())
			setGeometry(_geometry);
		comboboxArea->findCombobox->setFocus();
	}
}

void FindDialog::hideEvent(QHideEvent *event)
{
	if (!event->spontaneous())
		_geometry = geometry();
	QDialog::hideEvent(event);
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
