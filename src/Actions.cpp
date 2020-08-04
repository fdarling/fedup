#include "Actions.h"
#include "LoadIcon.h"

#include <QAction>

#include "FScintilla.h"

namespace fedup {

QAction * Actions::GenAction(const QString &name, const QString &seq, const QString &iconFilename)
{
    QAction * const action = new QAction(LoadIcon(iconFilename), name, this);
    action->setShortcut(QKeySequence(seq));
    return action;
}

Actions::Actions(QObject *parent) : QObject(parent)
{
    fileNew      = GenAction("&New",        "Ctrl+N",     "newFile.bmp");
    fileOpen     = GenAction("&Open",       "Ctrl+O",     "openFile.bmp");
    fileReload   = GenAction("Re&load from Disk", "", "");
    fileSave     = GenAction("&Save",       "Ctrl+S",     "saved.ico");
    fileSaveAs   = GenAction("Save &As...", "Ctrl+Alt+S", "");
    fileClose    = GenAction("&Close",      "Ctrl+W",     "closeFile.bmp");
    fileCloseAll = GenAction("Clos&e All",  "",           "closeAll.bmp");
    fileExit     = GenAction("E&xit",       "Alt+F4",     ""); // TODO "style()->standardIcon(QStyle::SP_TitleBarCloseButton)" and "QKeySequence::Quit"

    editUndo               = GenAction("&Undo",                  "Ctrl+Z",          "undo.bmp");
    editRedo               = GenAction("&Redo",                  "Ctrl+Alt+Z",      "redo.bmp");
    editCut                = GenAction("Cu&t",                   "Ctrl+X",          "cut.bmp");
    editCopy               = GenAction("&Copy",                  "Ctrl+C",          "copy.bmp");
    editPaste              = GenAction("&Paste",                 "Ctrl+V",          "paste.bmp");
    editDelete             = GenAction("&Delete",                "",                "delete.ico");
    editSelectAll          = GenAction("Select A&ll",            "Ctrl+A",          "");
    editDuplicateLines     = GenAction("Duplicate current line", "Ctrl+D",          "");
    editDeleteLine         = GenAction("Delete current line",    "Ctrl+K",          "");
    editUndeleteLines      = GenAction("Undelete lines",         "Ctrl+U",          "");
    editMoveLineUp         = GenAction("Move Up Current Line",   "Ctrl+Shift+Up",   "");
    editMoveLineDown       = GenAction("Move Down Current Line", "Ctrl+Shift+Down", "");
    editToggleBlockComment = GenAction("Toggle Block Comment",   "Ctrl+Q",          "");
    editEolWindows         = GenAction("Windows Format",               "",          "win.png");
    editEolUnix            = GenAction("UNIX Format",                  "",          "tux.png");
    editEolMac             = GenAction("Mac Format",                   "",          "mac.png");
    editTrimTrailingSpaces = GenAction("Trim trailing spaces",   "",                "");
    editSimplifyWhitespace = GenAction("Simplify Whitespace",    "",                "");
    editEolWindows->setCheckable(true);
    editEolUnix->setCheckable(true);
    editEolMac->setCheckable(true);
    editEolWindows->setData(FScintilla::EolWindows);
    editEolUnix->setData(FScintilla::EolUnix);
    editEolMac->setData(FScintilla::EolMac);
    editEolGroup = new QActionGroup(this);
    editEolGroup->addAction(editEolWindows);
    editEolGroup->addAction(editEolUnix);
    editEolGroup->addAction(editEolMac);
    editEolGroup->setExclusive(true);

    searchFind = GenAction("&Find...", "Ctrl+F", "find.bmp");
    searchFindInFiles = GenAction("Find in Files...", "Ctrl+Shift+F", "");
    searchFindNext = GenAction("Find &Next...", "F3", "");
    searchFindPrev = GenAction("Find &Previous...", "Shift+F3", "");
    searchReplace = GenAction("&Replace...", "Ctrl+R", "findReplace.bmp");
    searchGoTo = GenAction("&Go to...", "Ctrl+G", "");
    searchGoToMatchingBrace = GenAction("Go to Matching Brace", "Ctrl+B", "");
    searchBookmarkToggle = GenAction("Toggle Bookmark", "Ctrl+F2", "");
    searchBookmarkNext = GenAction("Next Bookmark", "F2", "");
    searchBookmarkPrev = GenAction("Prev Bookmark", "Shift+F2", "");
    searchBookmarkClearAll = GenAction("Clear All Bookmarks", "", "");

    viewAlwaysOnTop = GenAction("Always on Top", "", "");
    viewFullscreen = GenAction("Toggle Fullscreen", "F11", "");
    viewSearchResults = GenAction("Search Results", "F7", "");
    viewSymbolWhitespace = GenAction("Show White Space and TAB", "", "");
    viewSymbolNewlines = GenAction("Show End of Line", "", "");
    viewIndentationGuide = GenAction("Show Indent Guide", "", "");
    viewFoldAll = GenAction("Fold All", "Alt+0", "");
    viewUnfoldAll = GenAction("Unfold All", "Alt+Shift+0", "");
    viewCollapse1 = GenAction("1", "Alt+1", "");
    viewCollapse2 = GenAction("2", "Alt+2", "");
    viewCollapse3 = GenAction("3", "Alt+3", "");
    viewCollapse4 = GenAction("4", "Alt+4", "");
    viewCollapse5 = GenAction("5", "Alt+5", "");
    viewCollapse6 = GenAction("6", "Alt+6", "");
    viewCollapse7 = GenAction("7", "Alt+7", "");
    viewCollapse8 = GenAction("8", "Alt+8", "");
    viewUncollapse1 = GenAction("1", "Alt+Shift+1", "");
    viewUncollapse2 = GenAction("2", "Alt+Shift+2", "");
    viewUncollapse3 = GenAction("3", "Alt+Shift+3", "");
    viewUncollapse4 = GenAction("4", "Alt+Shift+4", "");
    viewUncollapse5 = GenAction("5", "Alt+Shift+5", "");
    viewUncollapse6 = GenAction("6", "Alt+Shift+6", "");
    viewUncollapse7 = GenAction("7", "Alt+Shift+7", "");
    viewUncollapse8 = GenAction("8", "Alt+Shift+8", "");
    viewAlwaysOnTop->setCheckable(true);
    viewFullscreen->setCheckable(true);
    viewSearchResults->setCheckable(true);
    viewSymbolWhitespace->setCheckable(true);
    viewSymbolNewlines->setCheckable(true);
    viewIndentationGuide->setCheckable(true);

    macroStartRecording = GenAction("Start Re&cording", "" , "startRecord.bmp");
    macroStopRecording = GenAction("S&top Recording", "" , "stopRecord.bmp");
    macroPlayback = GenAction("&Playback", "" , "playRecord.bmp");
    macroRunMultiple = GenAction("&Run Macro Multiple Times", "" , "playRecord_m.bmp");
}

Actions::~Actions()
{
}

} // namespace fedup
