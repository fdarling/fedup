#include "FScintilla.h"

#include <QScrollBar>

// HACK, not included in the Qt port?
#ifndef SCE_UNIVERSAL_FOUND_STYLE_SMART
#define SCE_UNIVERSAL_FOUND_STYLE_SMART 29
#endif

#if (QT_POINTER_SIZE == 4)
struct CharacterRange {
	qint32 cpMin;
	qint32 cpMax;
};
#else
struct CharacterRange {
	qint64 cpMin;
	qint64 cpMax;
};
#endif
struct TextToFind {
	CharacterRange chrg;     // range to search
	char *lpstrText;                // the search pattern (zero terminated)
	CharacterRange chrgText; // returned as position of matching text
};

namespace fedup {

static const int FSCINTILLA_ACTIVELINE_MARKER_ID = 15;
static const int FSCINTILLA_ACTIVELINE_MARKER_BACKGROUND_COLOR = (232) | (232 << 8) | (255 << 16);
static const int FSCINTILLA_SMARTHIGHLIGHTING_BACKGROUND_COLOR = (0) | (255 << 8) | (0 << 16);

static const int MAXLINEHIGHLIGHT = 400; // hardcoded max lines to rehilight at once for performance considerations ???

static bool IsWord(const QString &word)
{
	for (int i = 0; i < word.size(); i++)
	{
		if (!word[i].isLetterOrNumber() && word[i] != '_')
			return false;
	}

	return true;
}

FScintilla::FScintilla(QWidget *parent) : QsciScintilla(parent), _selectionLength(0), _lineCount(0), _length(0), _currentLine(0), _currentOffset(0), _undoAvailable(false), _redoAvailable(false)
{
	setMarginLineNumbers(1, true);
	setMarginWidth(1, 50);
	setMarginsForegroundColor(Qt::darkGray);

	setFolding(BoxedTreeFoldStyle, 2);
	setAutoIndent(true);
	setIndentationGuides(true);

	// setAutoCompletionThreshold(6);
	// setAutoCompletionSource(AcsAll);

	// TODO these should be user configurable settings
	setTabWidth(4);

	{
		QFont f("Courier New", 10);
		// f.setStyleStrategy(QFont::PreferBitmap);
		// f.setStyleStrategy(QFont::PreferDevice);
		f.setStyleStrategy(QFont::NoAntialias);
		f.setStyleHint(QFont::TypeWriter);
		setFont(f);
		setMarginsFont(f);
	}
	//SendScintilla(SCI_SETFONTQUALITY, SC_EFF_QUALITY_NON_ANTIALIASED); // TODO make a preprocessor if to detect windows and enable this

	// SendScintilla(SCI_SETHSCROLLBAR, false);
	// SendScintilla(SCI_SETSCROLLWIDTH, 800);
	// SendScintilla(SCI_SETSCROLLWIDTHTRACKING, true);

	SendScintilla(SCI_SETTWOPHASEDRAW, true);
	SendScintilla(SCI_INDICSETSTYLE, SCE_UNIVERSAL_FOUND_STYLE_SMART, INDIC_ROUNDBOX);
	SendScintilla(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE_SMART, true);
	SendScintilla(SCI_INDICSETALPHA, SCE_UNIVERSAL_FOUND_STYLE_SMART, (long int)100);
	// SendScintilla(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_SMART, FSCINTILLA_SMARTHIGHLIGHTING_BACKGROUND_COLOR);
	SendScintilla(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_SMART, FSCINTILLA_SMARTHIGHLIGHTING_BACKGROUND_COLOR);

	// SendScintilla(SCI_SETCARETLINEBACK, );
	// SendScintilla(SCI_SETCARETLINEBACK, 255);
	// SendScintilla(SCI_SETCARETLINEBACKALPHA, 128);
	SendScintilla(SCI_MARKERDEFINE, FSCINTILLA_ACTIVELINE_MARKER_ID, SC_MARK_BACKGROUND);
	SendScintilla(SCI_MARKERSETBACK, FSCINTILLA_ACTIVELINE_MARKER_ID, FSCINTILLA_ACTIVELINE_MARKER_BACKGROUND_COLOR);

	/*SendScintilla(SCI_MARKERDEFINE, EDITPANE_ERROR_MARKER_ID, SC_MARK_BACKGROUND);
	SendScintilla(SCI_MARKERSETBACK, EDITPANE_ERROR_MARKER_ID, EDITPANE_ERROR_BACKGROUND_COLOR);*/

	setModified(false);

	connect((QObject*)verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(_slot_Scrolled())); // NOTE: cast necessary because QScrollBar has only been forward declared, we don't know that it inherits QObject
	connect(this, SIGNAL(textChanged()), this, SLOT(_slot_TextChanged()));
	connect(this, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(_slot_CursorPositionChanged(int, int)));
	connect(this, SIGNAL(selectionChanged()), this, SLOT(_slot_SelectionChanged()));

	// TODO put this kind of initialization stuff into a private member function
	SendScintilla(SCI_MARKERADD, _currentLine, FSCINTILLA_ACTIVELINE_MARKER_ID);
}

FScintilla::~FScintilla()
{
}

void FScintilla::setDocument(const QsciDocument &doc)
{
	const bool wasModified = isModified();
	QsciScintilla::setDocument(doc);

	// TODO figure out why the font keeps getting reset!
	QFont f("Courier New", 10);
	setFont(f);
	setMarginsFont(f);
	setTabWidth(4);

	const bool nowModified = isModified();
	if (wasModified != nowModified)
		emit modificationChanged(nowModified); // HACK workaround for QsciScintilla not doing this for us >.<
	_RefreshCurrentLineMarker(_currentLine);
	_slot_TextChanged();
}

void FScintilla::goToLine(int line)
{
	if (line < 0)
		line = 0;
	else if (line >= lines())
		line = lines() - 1;
	setCursorPosition(line, 0);
}

void FScintilla::goToOffset(int offset)
{
	// TODO the cursor can currently get stuck "between" characters when landing on a CRLF, should it snap forward? That would visually look correct at least
	if (offset < 0)
		offset = 0;
	else if (offset > _length)
		offset = _length;
	SendScintilla(SCI_GOTOPOS, offset);
}

void FScintilla::duplicateLines()
{
	int selectionStartPos = SendScintilla(SCI_GETSELECTIONSTART);
	int selectionEndPos = SendScintilla(SCI_GETSELECTIONEND);
	const int selectionStartLine = SendScintilla(SCI_LINEFROMPOSITION, selectionStartPos);
	const int selectionEndLine = SendScintilla(SCI_LINEFROMPOSITION, selectionEndPos);
	if (selectionStartLine == selectionEndLine)
	{
		SendScintilla(SCI_LINEDUPLICATE);
	}
	else
	{
		beginUndoAction();
		int oldLine = -1, oldIndex = 0;
		int oldLine1 = -1, oldLine2 = -1, oldIndex1 = 0, oldIndex2 = 0;
		getCursorPosition(&oldLine, &oldIndex);
		getSelection(&oldLine1, &oldIndex1, &oldLine2, &oldIndex2);
		setSelection(selectionStartLine, 0, selectionEndLine + 1, 0);
		const QString selText = selectedText();
		insertAt(selText, selectionEndLine + 1, 0);
		setCursorPosition(oldLine, oldIndex);
		setSelection(oldLine1, oldIndex1, oldLine2, oldIndex2);
		endUndoAction();
	}
}

void FScintilla::deleteLine()
{
	int line = -1, oldColumn = 0;
	getCursorPosition(&line, &oldColumn);
	const int linelength = lineLength(line);

	beginUndoAction();
	setSelection(line, 0, line, linelength);
	removeSelectedText();
	setCursorPosition(line, oldColumn);
	endUndoAction();
}

void FScintilla::moveLineUp()
{
	if (hasSelectedText())
	{
		// determine the ranges that were selecting, and where the cursor is
		int oldLine = -1, oldColumn = 0;
		int selectionStartLine = -1, selectionEndLine = -1, selectionStartColumn = 0, selectionEndColumn = 0;
		getCursorPosition(&oldLine, &oldColumn);
		getSelection(&selectionStartLine, &selectionStartColumn, &selectionEndLine, &selectionEndColumn);

		// we can't go off the top!
		if (selectionStartLine == 0)
			return;

		beginUndoAction();
		for (int i = selectionStartLine; i <= selectionEndLine; i++)
		{
			// printf("Transposing line %3i\n", i+1);
			setCursorPosition(i, 0);
			SendScintilla(SCI_LINETRANSPOSE);
		}
		setCursorPosition(oldLine - 1, oldColumn);
		setSelection(selectionStartLine - 1, selectionStartColumn, selectionEndLine - 1, selectionEndColumn);
		endUndoAction();
	}
	else
	{
		// TODO use the C++ equivalent functions where possible
		int currentLine = SendScintilla(SCI_LINEFROMPOSITION, SendScintilla(SCI_GETCURRENTPOS));
		const int currentColumn = SendScintilla(SCI_GETCOLUMN, SendScintilla(SCI_GETCURRENTPOS));
		if (currentLine != 0)
		{
			SendScintilla(SCI_BEGINUNDOACTION);
			currentLine--;
			SendScintilla(SCI_LINETRANSPOSE);
			SendScintilla(SCI_GOTOPOS, SendScintilla(SCI_FINDCOLUMN, currentLine, currentColumn));
			SendScintilla(SCI_ENDUNDOACTION);
		}
	}
}

void FScintilla::moveLineDown()
{
	if (hasSelectedText())
	{
		// determine the ranges that were selecting, and where the cursor is
		int oldLine = -1, oldColumn = 0;
		int selectionStartLine = -1, selectionEndLine = -1, selectionStartColumn = 0, selectionEndColumn = 0;
		getCursorPosition(&oldLine, &oldColumn);
		getSelection(&selectionStartLine, &selectionStartColumn, &selectionEndLine, &selectionEndColumn);

		// we can't go off the bottom!
		if (selectionEndLine == lines() - 1)
			return;

		beginUndoAction();
		for (int i = selectionEndLine + 1; i > selectionStartLine; i--)
		{
			// printf("Transposing line %3i\n", i+1);
			setCursorPosition(i, 0);
			SendScintilla(SCI_LINETRANSPOSE);
		}
		setCursorPosition(oldLine + 1, oldColumn);
		setSelection(selectionStartLine + 1, selectionStartColumn, selectionEndLine + 1, selectionEndColumn);
		endUndoAction();
	}
	else
	{
		// TODO use the C++ equivalent functions where possible
		int currentLine = SendScintilla(SCI_LINEFROMPOSITION, SendScintilla(SCI_GETCURRENTPOS));
		const int currentColumn = SendScintilla(SCI_GETCOLUMN, SendScintilla(SCI_GETCURRENTPOS));
		if (currentLine != (SendScintilla(SCI_GETLINECOUNT) - 1))
		{
			SendScintilla(SCI_BEGINUNDOACTION);
			currentLine++;
			SendScintilla(SCI_GOTOLINE, currentLine);
			SendScintilla(SCI_LINETRANSPOSE);
			SendScintilla(SCI_GOTOPOS, SendScintilla(SCI_FINDCOLUMN, currentLine, currentColumn));
			SendScintilla(SCI_ENDUNDOACTION);
		}
	}
}

void FScintilla::toggleCommented()
{
	int selectionStartPos = SendScintilla(SCI_GETSELECTIONSTART);
	int selectionEndPos = SendScintilla(SCI_GETSELECTIONEND);
	const int selectionStartLine = SendScintilla(SCI_LINEFROMPOSITION, selectionStartPos);
	const int selectionEndLine = SendScintilla(SCI_LINEFROMPOSITION, selectionEndPos);

	static const char commentStr[] = "//";
	static const int commentLength = sizeof(commentStr) - 1;

	// TODO properly handle partially selected lines where only whitespace is selected (we shouldn't toggle the whole line on account of that)
	SendScintilla(SCI_BEGINUNDOACTION);
	for (int line = selectionStartLine; line <= selectionEndLine; line++)
	{
		const QString lineText = text(line).trimmed();
		if (lineText.size() == 0)
		{
			continue;
		}
		else if (lineText.startsWith(commentStr))
		{
			const int removeFrom = SendScintilla(SCI_GETLINEINDENTPOSITION, line);
			SendScintilla(SCI_SETSEL, removeFrom, removeFrom + commentLength);
			SendScintilla(SCI_REPLACESEL, (long unsigned int)0, (const char*)"");
			if (selectionStartPos >= removeFrom + commentLength)
				selectionStartPos -= commentLength;
			else if (selectionStartPos > removeFrom)
				selectionStartPos = removeFrom;
			if (selectionEndPos >= removeFrom + commentLength)
				selectionEndPos -= commentLength;
			else if (selectionEndPos > removeFrom)
				selectionEndPos = removeFrom;
		}
		else
		{
			const int insertAt = SendScintilla(SCI_GETLINEINDENTPOSITION, line);
			SendScintilla(SCI_INSERTTEXT, insertAt, commentStr);
			if (selectionStartPos > insertAt)
				selectionStartPos += commentLength;
			if (selectionEndPos > insertAt)
				selectionEndPos += commentLength;
		}
	}
	SendScintilla(SCI_SETSEL, selectionStartPos, selectionEndPos);
	SendScintilla(SCI_ENDUNDOACTION);

	//SCI_GETTEXTRANGE, SCI_VCHOME, SCI_LINEEND
}

void FScintilla::trimTrailingWhitespace()
{
	beginUndoAction();
	const int scrollPos = verticalScrollBar()->value();
	const int currentLine = SendScintilla(SCI_LINEFROMPOSITION, SendScintilla(SCI_GETCURRENTPOS));
	const int currentColumn = SendScintilla(SCI_GETCOLUMN, SendScintilla(SCI_GETCURRENTPOS));
	while (true)
	{
		//printf("Finding trailing spaces...");
		const bool result = findFirst("[ \t]+$", true, true, false, false, true, 0, 0, false);
		if (!result)
		{
			//printf("Done!\n");
			break;
		}
		//printf("found...\n");

		replace("");
	}
	SendScintilla(SCI_GOTOPOS, SendScintilla(SCI_FINDCOLUMN, currentLine, currentColumn));
	verticalScrollBar()->setValue(scrollPos);
	endUndoAction();
}

void FScintilla::_RefreshCurrentLineMarker(int newLine)
{
	SendScintilla(SCI_MARKERDELETEALL, FSCINTILLA_ACTIVELINE_MARKER_ID);
	SendScintilla(SCI_MARKERADD, newLine, FSCINTILLA_ACTIVELINE_MARKER_ID);
}

void FScintilla::_RefreshSmartHighlighting()
{
	_ClearSmartHighlighting();
	_smartHighlightedRange.clear();
}

void FScintilla::_ClearSmartHighlighting()
{
	const int documentLength = SendScintilla(SCI_GETLENGTH);
	SendScintilla(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_FOUND_STYLE_SMART);
	SendScintilla(SCI_INDICATORCLEARRANGE, 0, documentLength);
}

void FScintilla::_SmartHighlight(int firstDocumentLine, int lastDocumentLine)
{
	// determine the range of characters to search for highlighting
	const int documentLength = SendScintilla(SCI_GETLENGTH);
	const int startPos = SendScintilla(SCI_POSITIONFROMLINE, firstDocumentLine);
	int endPos = SendScintilla(SCI_POSITIONFROMLINE, lastDocumentLine + 1);
	if (endPos < 0 || endPos > documentLength - 1)
		endPos = documentLength - 1;
	if (endPos < startPos)
		return;

	// pass the character range and search text to scintilla via this structure
	TextToFind ttf;
	ttf.chrg.cpMin = startPos;
	ttf.chrg.cpMax = endPos;
	ttf.lpstrText = _smartHighlightedRange.word.data();

	// while we keep finding results, search the ever decreasing range of text
	while (true)
	{
		// return;
		const int result = SendScintilla(SCI_FINDTEXT, SCFIND_WHOLEWORD|SCFIND_MATCHCASE, &ttf);
		if (result < 0)
			break;

		// start searching again starting from the end of the last result
		ttf.chrg.cpMin = ttf.chrgText.cpMax+1;

		// highlight the match
		SendScintilla(SCI_INDICATORFILLRANGE, ttf.chrgText.cpMin, ttf.chrgText.cpMax - ttf.chrgText.cpMin);
	}
}

void FScintilla::_slot_Scrolled()
{
	if (!_smartHighlightedRange.highlighting())
		return;

	const int firstVisibleLine = SendScintilla(SCI_GETFIRSTVISIBLELINE);
	const int lastVisibleLine = firstVisibleLine + qMin((int)SendScintilla(SCI_LINESONSCREEN), MAXLINEHIGHLIGHT);
	int firstDocumentLine = SendScintilla(SCI_DOCLINEFROMVISIBLE, firstVisibleLine);
	int lastDocumentLine = SendScintilla(SCI_DOCLINEFROMVISIBLE, lastVisibleLine);

	// printf("Scrolled, showing [%i, %i]\n", firstDocumentLine, lastDocumentLine);

	// if we exposed more above
	if (firstDocumentLine < _smartHighlightedRange.firstLine)
	{
		_SmartHighlight(firstDocumentLine, _smartHighlightedRange.firstLine - 1);
		_smartHighlightedRange.firstLine = firstDocumentLine;
	}

	// if we exposed more below
	if (lastDocumentLine > _smartHighlightedRange.lastLine)
	{
		_SmartHighlight(_smartHighlightedRange.lastLine + 1, lastDocumentLine);
		_smartHighlightedRange.lastLine = lastDocumentLine;
	}
}

void FScintilla::_slot_TextChanged()
{
	{
		const int newLineCount = lines();
		const bool needsNotification = (newLineCount != _lineCount);
		_lineCount = newLineCount;
		if (needsNotification)
			emit lineCountChanged(newLineCount);
	}
	{
		const int newLength = length();
		const bool needsNotification = (newLength != _length);
		_length = newLength;
		if (needsNotification)
			emit lengthChanged(newLength);
	}
	{
		const bool newUndoAvailable = isUndoAvailable();
		const bool needsNotification = (newUndoAvailable != _undoAvailable);
		_undoAvailable = newUndoAvailable;
		if (needsNotification)
			emit undoAvailable(newUndoAvailable);
	}
	{
		const bool newRedoAvailable = isRedoAvailable();
		const bool needsNotification = (newRedoAvailable != _redoAvailable);
		_redoAvailable = newRedoAvailable;
		if (needsNotification)
			emit redoAvailable(newRedoAvailable);
	}
}

void FScintilla::_slot_CursorPositionChanged(int line, int index)
{
	Q_UNUSED(index);
	const int newPos = SendScintilla(SCI_GETCURRENTPOS);
	const int newLine = line;
	const bool needsNotification = (newPos != _currentOffset);
	const bool needsNotification2 = (newLine != _currentLine);

	if (needsNotification2)
		_RefreshCurrentLineMarker(newLine);

	_currentOffset = newPos;
	_currentLine = newLine;
	if (needsNotification)
		emit positionChanged(newPos);
	if (needsNotification2)
		emit lineChanged(line);
}

void FScintilla::_slot_SelectionChanged()
{
	const int newSelectionLength = SendScintilla(SCI_GETSELECTIONEND) - SendScintilla(SCI_GETSELECTIONSTART);
	const bool needsNotification = (newSelectionLength != _selectionLength);
	_selectionLength = newSelectionLength;
	if (needsNotification)
		emit selectionLengthChanged(newSelectionLength);

	const QString wordStr = selectedText();
	if (_selectionLength > 1000) // word length sanity check
		return;
	if (wordStr == _smartHighlightedRange.word)
		return;
	_smartHighlightedRange.word = wordStr.toAscii();

	// clear the currently smart-highlighted words
	_ClearSmartHighlighting();

	// only continue if we have an actual word to search for
	QByteArray word = wordStr.toAscii();
	if (wordStr.size() == 0 || !IsWord(wordStr))
		return;

	// determine the range of lines to search for highlighting
	const int firstVisibleLine = SendScintilla(SCI_GETFIRSTVISIBLELINE);
	const int lastVisibleLine = firstVisibleLine + std::min((int)SendScintilla(SCI_LINESONSCREEN), MAXLINEHIGHLIGHT);
	const int firstDocumentLine = SendScintilla(SCI_DOCLINEFROMVISIBLE, firstVisibleLine);
	const int lastDocumentLine = SendScintilla(SCI_DOCLINEFROMVISIBLE, lastVisibleLine);

	_smartHighlightedRange.firstLine = firstDocumentLine;
	_smartHighlightedRange.lastLine = lastDocumentLine;
	_SmartHighlight(firstDocumentLine, lastDocumentLine);
}

} // namespace fedup
