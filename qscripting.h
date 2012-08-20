#ifndef QSCRIPTING_H
#define QSCRIPTING_H

#include <QObject>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScriptEngine>


class QScriptWindow;

class QEmulator : public QObject
{
    Q_OBJECT
public:
    QEmulator(QScriptWindow * window) : m_window(window) { }

public:


public slots:
    /**
      Resets the emulator.
      */
    void reset();
    /**
      Runs the emulator for the given number of frames. One second is 25 frames.
      */
    bool run(int frames);
    /**
      Sets breakpoint address for the next run() call.
      */
    void setBreakpoint(quint16 address);
    /**
      Check if the emulator stopped on a breakpoint.
      */
    bool isBreakpoint();
    /**
      Save screenshot to PNG file.
      */
    void saveScreenshot(const QString& filename);
    /**
      Load the cartridge image file.
      */
    bool attachCartridge(int slot, const QString& filename);
    /**
      Empty the cartridge slot.
      */
    void detachCartridge(int slot);
    /**
      Attach the floppy image file -- insert the disk.
      */
    bool attachFloppy(int slot, const QString& filename);
    /**
      Detach the floppy image file -- remove the disk.
      */
    void detachFloppy(int slot);

    //TODO: Keyboard keys press/release
    //TODO: getUptime()
    //TODO: Attach/detach hard disk images
    //TODO: CPU/PPU registers, memory
    //TODO: Change screen modes, sound on/off

private:
    QScriptWindow * m_window;
};


class QScriptWindow : public QDialog
{
    Q_OBJECT

public:
    QScriptWindow(QWidget * parent = 0);
    ~QScriptWindow();

public:
    void runScript(const QString & script);
    bool isAborted() const { return m_aborted; }

public slots:
    void reject();

private slots:
    void cancelButtonPressed();

private:
    QVBoxLayout m_vlayout;
    QLabel m_static;
    QPushButton m_cancelButton;
    QScriptEngine m_engine;
    bool m_aborted;
    QEmulator * m_emulator;
};


#endif //QSCRIPTING_H
