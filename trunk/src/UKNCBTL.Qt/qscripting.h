#ifndef QSCRIPTING_H
#define QSCRIPTING_H

#include <QObject>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScriptEngine>


class QScriptWindow;
class CProcessor;


class QEmulatorProcessor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName)
    Q_PROPERTY(ushort sp READ getSP)
    Q_PROPERTY(ushort pc READ getPC)
    Q_PROPERTY(ushort psw READ getPSW)
    Q_PROPERTY(bool halt READ isHalt)
public:
    QEmulatorProcessor(QScriptEngine* engine, CProcessor* processor);

public slots:
    /**
      Get the name of the processor: "CPU" or "PPU".
      */
    QString getName();
    /**
      Get register value.
      */
    ushort getReg(int regno);
    ushort r(int regno) { return getReg(regno); }
    ushort getSP() { return getReg(6); }
    ushort getPC() { return getReg(7); }
    ushort getPSW();
    /**
      Get HALT/USER mode flag.
      */
    bool isHalt();
    /**
      Read word from the processor memory.
      */
    ushort readWord(ushort addr);

    //TODO: uchar readByte(ushort addr);

    /**
      Disassemble one instruction at the given address.
      Return value is array: { address, instruction, arguments, instruction length }.
      */
    QScriptValue disassemble(ushort addr);

private:
    QScriptEngine* m_engine;
    CProcessor* m_processor;
};


class QEmulator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float uptime READ getUptime)
    Q_PROPERTY(QObject* cpu READ getCPU)
    Q_PROPERTY(QObject* ppu READ getPPU)
public:
    QEmulator(QScriptWindow * window);

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
      Get emulator uptime, in seconds.
      */
    float getUptime();
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
    /**
      Attach the hard drive image file.
      */
    bool attachHard(int slot, const QString& filename);
    /**
      Detach the hard drive image file.
      */
    void detachHard(int slot);
    /**
      Get CPU object.
      */
    QObject* getCPU() { return &m_cpu; }
    /**
      Get PPU object.
      */
    QObject* getPPU() { return &m_ppu; }
    /**
      Press the key (by scan code), wait timeout frames, release the key, wait 3 frames.
      */
    void keyScan(uchar ukncscan, int timeout = 3);
    /**
      Press SHIFT, press the key, wait timeout frames, release the key, release SHIFT, wait 3 frames.
      */
    void keyScanShift(uchar ukncscan, int timeout = 3);
    /**
      Type the key sequence.
      */
    void keyString(QString str);

    //TODO: Change screen modes, sound on/off
    //TODO: Save/restore state
    //TODO: Debugger: Step Into, Step Over

private:
    void keyChar(char ch, int timeout = 3);

private:
    QScriptWindow * m_window;
    QEmulatorProcessor m_cpu;
    QEmulatorProcessor m_ppu;
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
    QScriptEngine* getEngine() { return &m_engine; }

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
