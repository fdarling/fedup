#include "SearchResultsDock.h"

namespace fedup {

static const int FSCINTILLA_SEARCHRESULTS_TERM_MARKER_ID = 15;
static const int FSCINTILLA_SEARCHRESULTS_FILE_MARKER_ID = 16;
static const int FSCINTILLA_SEARCHRESULTS_TERM_BACKGROUND_COLOR = (187) | (187 << 8) | (255 << 16);
static const int FSCINTILLA_SEARCHRESULTS_FILE_BACKGROUND_COLOR = (213) | (255 << 8) | (213 << 16);
static const int FSCINTILLA_SEARCHRESULTS_FILE_FOREGROUND_COLOR = (  0) | (128 << 8) | (  0 << 16);
// static const int FSCINTILLA_SEARCHRESULTS_TERM_FOREGROUND_COLOR = (0) | (255 << 8) | (0 << 16);

SearchResultsDock::SearchResultsDock(QWidget *parent) : QDockWidget("Search results", parent), _editor(new QsciScintilla), _filePathLine(0), _currentLine(0), _fileCount(0), _hitCount(0), _totalHits(0)
{
	_editor->setReadOnly(true);
	QFont f("Courier New", 10);
	_editor->setFont(f);
	_editor->setTabWidth(4);
	/*_editor->setIndicatorBackgroundColor(QColor(187, 187, 255), 1);
	_editor->setIndicatorForegroundColor(QColor(  0,   0, 128), 1);
	_editor->setIndicatorBackgroundColor(QColor(213, 255, 213), 2);
	_editor->setIndicatorForegroundColor(QColor(  0, 128,   0), 2);
	_editor->setIndicatorBackgroundColor(QColor(255, 255, 191), 3);
	_editor->setIndicatorForegroundColor(QColor(255,   0,   0), 3);*/
	_editor->SendScintilla(QsciScintillaBase::SCI_MARKERDEFINE, FSCINTILLA_SEARCHRESULTS_TERM_MARKER_ID, QsciScintillaBase::SC_MARK_BACKGROUND);
	_editor->SendScintilla(QsciScintillaBase::SCI_MARKERSETBACK, FSCINTILLA_SEARCHRESULTS_TERM_MARKER_ID, FSCINTILLA_SEARCHRESULTS_TERM_BACKGROUND_COLOR);
	_editor->SendScintilla(QsciScintillaBase::SCI_MARKERDEFINE, FSCINTILLA_SEARCHRESULTS_FILE_MARKER_ID, QsciScintillaBase::SC_MARK_BACKGROUND);
	_editor->SendScintilla(QsciScintillaBase::SCI_MARKERSETBACK, FSCINTILLA_SEARCHRESULTS_FILE_MARKER_ID, FSCINTILLA_SEARCHRESULTS_FILE_BACKGROUND_COLOR);

	startSearch("sting");
	addResult("NoWhere.CPP", 13, "\tTesting 1 2 3!\nWow...", 3, 5);
	addResult("NoWhere.CPP", 15, "\tTesting XXX", 3, 5);
	addResult("NoWhere.CPP", 18, "\tTesting 1 2 3!\nHmm...\nYup", 3, 5);
	addResult("NoOne.CPP", 3, "\tTesting 1 2 3!\nWow...", 3, 5);
	addResult("NoOne.CPP", 18, "\tTesting 1 2 3!\nWow...", 3, 5);
	addResult("Wicked.CPP", 4, "\tTesting 1 2 3!\nHmm...", 3, 5);
	endSearch();

	setWidget(_editor);
}

SearchResultsDock::~SearchResultsDock()
{
}

void SearchResultsDock::startSearch(const QString &term)
{
	_editor->insertAt("Search \"" + term + "\"\n", 0, 0);
	_editor->SendScintilla(QsciScintillaBase::SCI_MARKERADD, 0, FSCINTILLA_SEARCHRESULTS_TERM_MARKER_ID);
	_filePathLine = 1;
	_currentLine = 1;
	_fileCount = 0;
	_hitCount = 0;
	_totalHits = 0;
}

void SearchResultsDock::addResult(const QString &filePath, int line, const QString &result, int highlightStart, int highlightLength)
{
	if (_lastFilePath != filePath)
	{
		_FinishFile();
		_lastFilePath = filePath;
		_filePathLine = _currentLine++;
		_editor->insertAt("  " + filePath + "\n", _filePathLine, 0);
		_editor->SendScintilla(QsciScintillaBase::SCI_MARKERADD, _filePathLine, FSCINTILLA_SEARCHRESULTS_FILE_MARKER_ID);
		_fileCount++;
		_hitCount = 0;
	}
	// TODO support multi-line hits
	const QString prefix = "  Line " + QString::number(line).rightJustified(6) + ":\t";
	QString textBlock = result;
	textBlock.replace('\n', QString('\n') + QString(' ').repeated(prefix.size()-1) + QString('\t'));
	_editor->insertAt(prefix + textBlock + '\n', _currentLine, 0);
	// _editor->fillIndicatorRange(_currentLine, prefix.size() + highlightStart, _currentLine, prefix.size() + highlightStart + highlightLength, 3);
	_currentLine = _currentLine + 1 + result.count('\n');
	_hitCount++;
	_totalHits++;
}

void SearchResultsDock::endSearch()
{
	_FinishFile();
	_editor->insertAt(" (" + QString::number(_totalHits) + " hits in " + QString::number(_fileCount) + " files)", 0, _editor->text(0).size()-1);
}

void SearchResultsDock::clear()
{
	_editor->setText("");
	// SendScintilla(SCI_MARKERDELETEALL, FSCINTILLA_ACTIVELINE_MARKER_ID);
}

void SearchResultsDock::_FinishFile()
{
	if (!_lastFilePath.isEmpty())
	{
		_editor->insertAt(" (" + QString::number(_hitCount) + " hits)", _filePathLine, _editor->text(_filePathLine).size()-1);
	}
}

} // namespace fedup
