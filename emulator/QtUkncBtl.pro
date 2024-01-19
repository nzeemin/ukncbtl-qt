# -------------------------------------------------
# Project created by QtCreator 2010-05-06T17:43:10
# -------------------------------------------------
TARGET = QtUkncBtl
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    util/WavPcmFile.cpp \
    Common.cpp \
    emubase/Processor.cpp \
    emubase/Memory.cpp \
    emubase/Hard.cpp \
    emubase/Floppy.cpp \
    emubase/Disasm.cpp \
    emubase/Board.cpp \
    emubase/SoundAY.cpp \
    Emulator.cpp \
    qscreen.cpp \
    qkeyboardview.cpp \
    Settings.cpp \
    qdebugview.cpp \
    qdisasmview.cpp \
    qconsoleview.cpp \
    qmemoryview.cpp \
    qsoundout.cpp \
    qscripting.cpp \
    UnitTests.cpp \
    qdialogs.cpp
HEADERS += mainwindow.h \
    stdafx.h \
    util/WavPcmFile.h \
    Common.h \
    emubase/Processor.h \
    emubase/Memory.h \
    emubase/Emubase.h \
    emubase/Defines.h \
    emubase/Board.h \
    Emulator.h \
    qscreen.h \
    qkeyboardview.h \
    main.h \
    qdebugview.h \
    qdisasmview.h \
    qconsoleview.h \
    qmemoryview.h \
    qsoundout.h \
    qscripting.h \
    UnitTests.h \
    qdialogs.h
FORMS += mainwindow.ui
RESOURCES += QtUkncBtl.qrc
RC_ICONS = images/ukncbtl.ico
QT += widgets
QT += script
QT += testlib 
QT += multimedia
DEFINES -= UNICODE _UNICODE
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11
ICON = images/ukncbtl.icns

TS_FILES = $$files(lang/*.ts, false)
lrelease.name = LRELEASE ${QMAKE_FILE_IN}
lrelease.commands = lrelease ${QMAKE_FILE_IN} -qm lang/${QMAKE_FILE_BASE}.qm
lrelease.output = ${QMAKE_FILE_BASE}.qm
lrelease.input = TS_FILES
lrelease.clean = lang/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease
