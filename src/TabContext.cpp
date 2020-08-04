#include "TabContext.h"
#include "FScintilla.h"

#include <QScrollBar>
#include <QFileInfo>

namespace fedup {

TabContext::TabContext() : verticalScrollBarPosition(0), horizontalScrollBarPosition(0), selectionStartPos(0), selectionEndPos(0), lexer(NULL)
{
}

TabContext::TabContext(const QsciDocument &doc) : document(doc), verticalScrollBarPosition(0), horizontalScrollBarPosition(0), selectionStartPos(0), selectionEndPos(0), lexer(NULL)
{
}

TabContext::~TabContext()
{
}

void TabContext::save(FScintilla *editor)
{
    selectionStartPos = editor->SendScintilla(QsciScintilla::SCI_GETANCHOR);
    selectionEndPos = editor->SendScintilla(QsciScintilla::SCI_GETCURRENTPOS);
    verticalScrollBarPosition = editor->verticalScrollBar()->value();
    horizontalScrollBarPosition = editor->horizontalScrollBar()->value();
    contractedFolds = editor->contractedFolds();
    lexer = editor->lexer();
}

void TabContext::restore(FScintilla *editor) const
{
    editor->setFilePath(filePath);
    editor->setDocument(document);
    editor->SendScintilla(QsciScintilla::SCI_SETSEL, selectionStartPos, selectionEndPos);
    editor->verticalScrollBar()->setValue(verticalScrollBarPosition);
    editor->horizontalScrollBar()->setValue(horizontalScrollBarPosition);

    editor->setLexer(lexer);
    editor->setContractedFolds(contractedFolds);
    // HACK this isn't the best place to put this...
    editor->setMatchedBraceForegroundColor(Qt::red);
    editor->setMatchedBraceBackgroundColor(Qt::white);
}

} // namespace fedup
