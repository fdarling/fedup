#ifndef FEDUP_ACTIONS_H
#define FEDUP_ACTIONS_H

#include <QObject>
#include <QAction>
#include <QActionGroup>

QT_BEGIN_NAMESPACE
// class QAction;
class QString;
QT_END_NAMESPACE

namespace fedup {

class Actions : public QObject
{
	Q_OBJECT
public:
	Actions(QObject *parent = NULL);
	~Actions();
public:
	QAction *fileNew;
	QAction *fileOpen;
	QAction *fileSave;
	QAction *fileSaveAs;
	QAction *fileClose;
	QAction *fileCloseAll;
	QAction *fileExit;

	QAction *editUndo;
	QAction *editRedo;
	QAction *editCut;
	QAction *editCopy;
	QAction *editPaste;
	QAction *editDelete;
	QAction *editSelectAll;
	QAction *editDuplicateLines;
	QAction *editDeleteLine;
	QAction *editUndeleteLines;
	QAction *editMoveLineUp;
	QAction *editMoveLineDown;
	QAction *editToggleBlockComment;
	QAction *editEolWindows;
	QAction *editEolUnix;
	QAction *editEolMac;
	QAction *editTrimTrailingSpaces;
	QAction *editSimplifyWhitespace;
	QActionGroup *editEolGroup;

	QAction *searchFind;
	QAction *searchFindInFiles;
	QAction *searchFindNext;
	QAction *searchFindPrev;
	QAction *searchReplace;
	QAction *searchGoTo;
	QAction *searchGoToMatchingBrace;
	QAction *searchBookmarkToggle;
	QAction *searchBookmarkNext;
	QAction *searchBookmarkPrev;
	QAction *searchBookmarkClearAll;

	QAction *viewAlwaysOnTop;
	QAction *viewFullscreen;
	QAction *viewSearchResults;
	QAction *viewSymbolWhitespace;
	QAction *viewSymbolNewlines;
	QAction *viewIndentationGuide;

	QAction *macroStartRecording;
	QAction *macroStopRecording;
	QAction *macroPlayback;
	QAction *macroRunMultiple;
protected:
	QAction * GenAction(const QString &name, const QString &seq, const QString &iconFilename);
};

} // namespace fedup

#endif
