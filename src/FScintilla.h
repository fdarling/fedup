#ifndef FEDUP_FSCINTILLA_H
#define FEDUP_FSCINTILLA_H

#include <Qsci/qsciscintilla.h>

namespace fedup {

class FScintilla : public QsciScintilla
{
	Q_OBJECT
public:
	FScintilla(QWidget *parent = NULL);
	~FScintilla();

	void setDocument(const QsciDocument &doc);
signals:
	void selectionLengthChanged(int length);
	void lineCountChanged(int lineCount);
	void lengthChanged(int newLength);
	void positionChanged(int pos);
	void lineChanged(int line);
	void undoAvailable(bool available);
	void redoAvailable(bool available);
public slots:
	void goToLine(int line);
	void goToOffset(int offset);
	void duplicateLines();
	void deleteLine();
	void undeleteLines();
	void moveLineUp();
	void moveLineDown();
	void toggleCommented();
	void trimTrailingWhitespace();
	void setWhitespaceVisible(bool vis);
protected slots:
	void _slot_Scrolled();
	void _slot_TextChanged();
	void _slot_CursorPositionChanged(int line, int index);
	void _slot_SelectionChanged();
protected:
	void wheelEvent(QWheelEvent *event);

	int _LineLength(int line);
	void _RefreshCurrentLineMarker();
	void _RefreshCurrentLineMarker(int newLine);
	void _RefreshSmartHighlighting();
	void _ClearSmartHighlighting();
	void _SmartHighlight(int firstDocumentLine, int lastDocumentLine);

	int _selectionLength;
	int _lineCount;
	int _length;
	int _currentLine;
	int _currentOffset;
	bool _undoAvailable;
	bool _redoAvailable;
	class SmartHighlightedRange
	{
	public:
		SmartHighlightedRange() : firstLine(-1), lastLine(-1) {}

		bool highlighting() const {return word.size() > 0 && word[0] != '\0';}
		void clear()
		{
			firstLine = -1;
			lastLine = -1;
			word.clear();
		}

		int firstLine;
		int lastLine;
		QByteArray word;
	} _smartHighlightedRange;
	int _lastDeletedLine;
	QString _deletedLines;
};

} // namespace fedup

#endif
