cmake_minimum_required(VERSION 3.16)

project(QtUkncBtl VERSION 0.1 LANGUAGES CXX)

SET(CMAKE_OSX_ARCHITECTURES "x86_64")

add_subdirectory(emubase)

find_package(Qt5 REQUIRED COMPONENTS Widgets LinguistTools Script Multimedia Test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    util/WavPcmFile.cpp
    Common.cpp
    Emulator.cpp
    qscreen.cpp
    qkeyboardview.cpp
    Settings.cpp
    qdebugview.cpp
    qdisasmview.cpp
    qconsoleview.cpp
    qmemoryview.cpp
    qsoundout.cpp
    qscripting.cpp
    qdialogs.cpp
    UnitTests.cpp
)
set(PROJECT_HEADERS
    mainwindow.h
    stdafx.h
    util/WavPcmFile.h
    Common.h
    Emulator.h
    qscreen.h
    qkeyboardview.h
    main.h
    qdebugview.h
    qdisasmview.h
    qconsoleview.h
    qmemoryview.h
    qsoundout.h
    qscripting.h
    qdialogs.h
    UnitTests.h
)

set ( PROJECT_UIS mainwindow.ui )
set ( PROJECT_RESOURCES QtUkncBtl.qrc )

qt5_create_translation(QM_FILES ${PROJECT_SOURCES} lang/ukncbtl_en.ts lang/ukncbtl_ru.ts)
configure_file(translations.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_executable(QtUkncBtl
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_UIS}
    ${PROJECT_RESOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
)

target_include_directories(QtUkncBtl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(QtUkncBtl PRIVATE emubase Qt5::Widgets Qt5::Script Qt5::Multimedia Qt5::Test)

set_target_properties(QtUkncBtl PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QtUkncBtl
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
