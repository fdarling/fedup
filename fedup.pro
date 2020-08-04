TEMPLATE = app
TARGET = fedup
QT += network widgets
# CONFIG += release
# CONFIG += console release
# CONFIG += console debug
INCLUDEPATH += src/qtsingleapplication
greaterThan(QT_MAJOR_VERSION, 4) {
    LIBS += -lqscintilla2_qt5
} else {
    LIBS += -lqscintilla2
}

RC_FILE = fedup.rc
RESOURCES = fedup.qrc
HEADERS = \
    src/qtsingleapplication/qtlocalpeer.h \
    src/qtsingleapplication/qtsingleapplication.h \
    src/globals.h \
    src/LoadIcon.h \
    src/LexerPicker.h \
    src/FileFilters.h \
    src/Actions.h \
    src/RecentFilesList.h \
    src/GoToDialog.h \
    src/FindDialog.h \
    src/ExitSaveDialog.h \
    src/TabContext.h \
    src/FScintilla.h \
    src/EditPaneTabs.h \
    src/EditPane.h \
    src/SearchResultsDock.h \
    src/StatusBar.h \
    src/ToolBar.h \
    src/MenuBar.h \
    src/MainWindow.h
SOURCES = \
    src/qtsingleapplication/qtlocalpeer.cpp \
    src/qtsingleapplication/qtsingleapplication.cpp \
    src/LoadIcon.cpp \
    src/LexerPicker.cpp \
    src/FileFilters.cpp \
    src/Actions.cpp \
    src/RecentFilesList.cpp \
    src/GoToDialog.cpp \
    src/FindDialog.cpp \
    src/ExitSaveDialog.cpp \
    src/TabContext.cpp \
    src/FScintilla.cpp \
    src/EditPaneTabs.cpp \
    src/EditPane.cpp \
    src/SearchResultsDock.cpp \
    src/StatusBar.cpp \
    src/ToolBar.cpp \
    src/MenuBar.cpp \
    src/MainWindow.cpp \
    src/main.cpp

CONFIG(release, debug|release) {
OBJECTS_DIR=build_release/obj
MOC_DIR=build_release/moc
RCC_DIR=build_release/rcc
}
CONFIG(debug, debug|release) {
OBJECTS_DIR=build_debug/obj
MOC_DIR=build_debug/moc
RCC_DIR=build_debug/rcc
TARGET = fedup_debug
}
