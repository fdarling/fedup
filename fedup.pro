TEMPLATE = app
TARGET = fedup
QT += network
CONFIG += console release
INCLUDEPATH += src/qtsingleapplication
LIBS += -lqscintilla2
RC_FILE = fedup.rc
RESOURCES = fedup.qrc
HEADERS = \
	src/qtsingleapplication/qtlocalpeer.h \
	src/qtsingleapplication/qtsingleapplication.h \
	src/LoadIcon.h \
	src/LexerPicker.h \
	src/Actions.h \
	src/GoToDialog.h \
	src/FindDialog.h \
	src/TabContext.h \
	src/FScintilla.h \
	src/EditPaneTabs.h \
	src/EditPane.h \
	src/StatusBar.h \
	src/ToolBar.h \
	src/MenuBar.h \
	src/MainWindow.h
SOURCES = \
	src/qtsingleapplication/qtlocalpeer.cpp \
	src/qtsingleapplication/qtsingleapplication.cpp \
	src/LoadIcon.cpp \
	src/LexerPicker.cpp \
	src/Actions.cpp \
	src/GoToDialog.cpp \
	src/FindDialog.cpp \
	src/TabContext.cpp \
	src/FScintilla.cpp \
	src/EditPaneTabs.cpp \
	src/EditPane.cpp \
	src/StatusBar.cpp \
	src/ToolBar.cpp \
	src/MenuBar.cpp \
	src/MainWindow.cpp \
	src/main.cpp

Release:DESTDIR = ./
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:DESTDIR = debug/
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui
