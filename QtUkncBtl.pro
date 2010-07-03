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
    Emulator.cpp \
    qscreen.cpp \
    qkeyboardview.cpp \
    Settings.cpp \
    qdebugview.cpp \
    qdisasmview.cpp \
    qconsoleview.cpp \
    qmemoryview.cpp
HEADERS += mainwindow.h \
    stdafx.h \
    util/WavPcmFile.h \
    Common.h \
    emubase/Processor.h \
    emubase/Memory.h \
    emubase/Emubase.h \
    emubase/Disasm.h \
    emubase/Defines.h \
    emubase/Board.h \
    Emulator.h \
    qscreen.h \
    qkeyboardview.h \
    main.h \
    qdebugview.h \
    qdisasmview.h \
    qconsoleview.h \
    qmemoryview.h
FORMS += mainwindow.ui
RESOURCES += QtUkncBtl.qrc
