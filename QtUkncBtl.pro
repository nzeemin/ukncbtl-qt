# -------------------------------------------------
# Project created by QtCreator 2010-05-06T17:43:10
# -------------------------------------------------
QT += multimedia
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
QT += widgets
QT += script
QT += testlib 
DEFINES -= UNICODE
