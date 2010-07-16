#ifndef QCONSOLEVIEW_H
#define QCONSOLEVIEW_H

#include <QWidget>
#include "Common.h"

class QTextEdit;
class QLineEdit;
class CProcessor;

class QConsoleView : public QWidget
{
    Q_OBJECT
public:
    QConsoleView();
    ~QConsoleView();

    void print(const QString &message);
    void printLine(const QString &message);
    void clear();
    void setCurrentProc(bool okProc);

public slots:
    void execConsoleCommand();
    void execConsoleCommand(const QString &command);

private:
    QTextEdit *m_log;
    QLineEdit *m_edit;
    bool m_okCurrentProc;

    CProcessor* getCurrentProcessor();
    void printConsolePrompt();
    void printHelp();
    int printDisassemble(CProcessor* pProc, WORD address, BOOL okOneInstr, BOOL okShort);
    void printRegister(LPCTSTR strName, WORD value);
    void printMemoryDump(CProcessor* pProc, WORD address, int lines);
};

#endif // QCONSOLEVIEW_H
