cmake_minimum_required(VERSION 3.13)

project(fedup LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED Widgets Network)

# TODO: find_package() equivalent to find QsciScintilla2... currently has no CMake nor pkg-config support :-(
set(QSCINTILLA_LIBRARIES qscintilla2_qt5)

if(WIN32)
    set(FEDUP_EXTRA_RESOURCES "fedup.rc")
else()
    set(FEDUP_EXTRA_RESOURCES "")
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/src/qtsingleapplication
)

add_executable(${PROJECT_NAME}
    src/qtsingleapplication/qtlocalpeer.cpp
    src/qtsingleapplication/qtsingleapplication.cpp
    src/LoadIcon.cpp
    src/LexerPicker.cpp
    src/FileFilters.cpp
    src/Actions.cpp
    src/RecentFilesList.cpp
    src/GoToDialog.cpp
    src/FindDialog.cpp
    src/ExitSaveDialog.cpp
    src/TabContext.cpp
    src/FScintilla.cpp
    src/EditPaneTabs.cpp
    src/EditPane.cpp
    src/SearchResultsDock.cpp
    src/StatusBar.cpp
    src/ToolBar.cpp
    src/MenuBar.cpp
    src/MainWindow.cpp
    src/main.cpp
    fedup.qrc
    ${FEDUP_EXTRA_RESOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Qt5::Network
    ${QSCINTILLA_LIBRARIES}
)
