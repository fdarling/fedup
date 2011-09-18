#ifndef FEDUP_TABCONTEXT_H
#define FEDUP_TABCONTEXT_H

#include <QString>
#include <QList>

#include <Qsci/qscidocument.h>
class QsciLexer;

namespace fedup {

class FScintilla;

class TabContext
{
public:
	TabContext();
	TabContext(const QsciDocument &doc);
	~TabContext();

	void save(FScintilla *editor);
	void restore(FScintilla *editor) const;

	QsciDocument document;
	QString filePath;
	int verticalScrollBarPosition;
	int horizontalScrollBarPosition;
	int selectionStartPos;
	int selectionEndPos;
	QsciLexer *lexer;
	QList<int> contractedFolds;
};

} // namespace fedup

#endif
