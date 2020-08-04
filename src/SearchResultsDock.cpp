#include "SearchResultsDock.h"

#include <QMouseEvent>
#include <QMenu>

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
    QFont f("monospace", 9);
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
    _editor->viewport()->installEventFilter(this);

    /*startSearch("sting");
    addResult("NoWhere.CPP", 13, "\tTesting 1 2 3!\nWow...", 3, 5);
    addResult("NoWhere.CPP", 15, "\tTesting XXX", 3, 5);
    addResult("NoWhere.CPP", 18, "\tTesting 1 2 3!\nHmm...\nYup", 3, 5);
    addResult("NoOne.CPP", 3, "\tTesting 1 2 3!\nWow...", 3, 5);
    addResult("NoOne.CPP", 18, "\tTesting 1 2 3!\nWow...", 3, 5);
    addResult("Wicked.CPP", 4, "\tTesting 1 2 3!\nHmm...", 3, 5);
    endSearch();*/

    setWidget(_editor);
}

SearchResultsDock::~SearchResultsDock()
{
}

void SearchResultsDock::startSearch(const QString &term)
{
    clear(); // TODO have clearing done from a context menu and not automatically every search, perhaps an option to do it every search though?
    _editor->insertAt("Search \"" + term + "\"\n", 0, 0);
    _editor->SendScintilla(QsciScintillaBase::SCI_MARKERADD, 0, FSCINTILLA_SEARCHRESULTS_TERM_MARKER_ID);
    _lastFilePath.clear();
    _filePathLine = 1;
    _currentLine = 1;
    _fileCount = 0;
    _hitCount = 0;
    _totalHits = 0;
}

void SearchResultsDock::addResult(const QString &filePath, int line, const QString &result, int highlightStart, int highlightLength)
{
    Q_UNUSED(highlightStart);
    Q_UNUSED(highlightLength);
    setVisible(true);
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
    const QString prefix = "  Line " + QString::number(line+1).rightJustified(6) + ":\t";
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
    _editor->setText(QString());
    // SendScintilla(SCI_MARKERDELETEALL, FSCINTILLA_ACTIVELINE_MARKER_ID);
}

static int FScintilla_PointToLineNumber(QsciScintilla *editor, const QPoint &pos)
{
    long chpos = editor->SendScintilla(QsciScintillaBase::SCI_POSITIONFROMPOINT, pos.x(), pos.y());
    if (chpos < 0)
        return -1;
    return editor->SendScintilla(QsciScintillaBase::SCI_LINEFROMPOSITION, chpos);
}

bool SearchResultsDock::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == _editor->viewport())
    {
        // TODO majorily clean this up, I need to store the filePath/lineNumber pairs in a more intelligent way, and also support double clicking on lines that aren't the first line in a multi-line hit
        if (event->type() == QEvent::MouseButtonDblClick)
        {
            QMouseEvent * const mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                const int lineClicked = FScintilla_PointToLineNumber(_editor, mouseEvent->pos());
                QString lineNumberString;
                if (lineClicked != -1)
                    lineNumberString = _editor->text(lineClicked);
                if (lineNumberString.startsWith("  Line ") && !(_editor->SendScintilla(QsciScintillaBase::SCI_MARKERGET, lineClicked) & ((1 << FSCINTILLA_SEARCHRESULTS_TERM_MARKER_ID) | (1 << FSCINTILLA_SEARCHRESULTS_FILE_MARKER_ID))))
                {
                    lineNumberString = lineNumberString.mid(7, 6).trimmed();
                    bool ok = false;
                    const int lineNumber = lineNumberString.toInt(&ok) - 1;
                    if (ok)
                    {
                        for (int i = lineClicked-1; i >= 0; i--)
                        {
                            const int markerFlags = _editor->SendScintilla(QsciScintillaBase::SCI_MARKERGET, i);
                            if (markerFlags & (1 << FSCINTILLA_SEARCHRESULTS_FILE_MARKER_ID))
                            {
                                QString filePath = _editor->text(i).mid(2);
                                filePath.chop(filePath.size() - filePath.lastIndexOf('(') + 1);
                                emit requestOpenFileLine(filePath, lineNumber);
                                return true;
                            }
                        }
                    }
                }
                return true;
            }
        }
        else if (event->type() == QEvent::ContextMenu)
        {
            QContextMenuEvent * const mouseEvent = static_cast<QContextMenuEvent*>(event);
            QMenu menu;
            QAction * const collapseAllAction    = menu.addAction("Collapse all");
            QAction * const uncollapseAllAction  = menu.addAction("Uncollapse all");
            menu.addSeparator();
            QAction * const copyAction           = menu.addAction("Copy");
            QAction * const selectAllAction      = menu.addAction("Select All");
            QAction * const clearAllAction       = menu.addAction("Clear All");
            menu.addSeparator();
            QAction * const openAllAction        = menu.addAction("Open All");
            collapseAllAction->setEnabled(false);
            uncollapseAllAction->setEnabled(false);
            openAllAction->setEnabled(false);
            QAction * const picked = menu.exec(mouseEvent->globalPos());

            if (picked == collapseAllAction || picked == uncollapseAllAction)
            {
                _editor->foldAll(true);
            }
            else if (picked == copyAction)
            {
                _editor->copy();
            }
            else if (picked == selectAllAction)
            {
                _editor->selectAll();
            }
            else if (picked == clearAllAction)
            {
                clear();
            }
            else if (picked == openAllAction)
            {
                // TODO
            }
            return true;
        }
    }
    return QDockWidget::eventFilter(obj, event);
}

void SearchResultsDock::_FinishFile()
{
    if (!_lastFilePath.isEmpty())
    {
        _editor->insertAt(" (" + QString::number(_hitCount) + " hits)", _filePathLine, _editor->text(_filePathLine).size()-1);
    }
}

} // namespace fedup
