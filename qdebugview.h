#ifndef QDEBUGVIEW_H
#define QDEBUGVIEW_H

#include <QWidget>
#include "Common.h"

class CMotherboard;
class CProcessor;
class CMemoryController;
class QToolBar;

class QDebugView : public QWidget
{
    Q_OBJECT
public:
    QDebugView(QWidget *mainWindow);

    void setCurrentProc(bool okProc);
    void updateData();
    void updateWindowText();

public slots:
    void switchCpuPpu();

protected:
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);

private:
    QToolBar* m_toolbar;
    bool m_okDebugProcessor;        // TRUE - CPU, FALSE - PPU
    unsigned short m_wDebugCpuR[9];  // Old register values - R0..R7, PSW
    unsigned short m_wDebugPpuR[9];  // Old register values - R0..R7, PSW
    bool m_okDebugCpuRChanged[9];   // Register change flags
    bool m_okDebugPpuRChanged[9];   // Register change flags

private:
    void drawProcessor(QPainter &painter, const CProcessor *pProc, int x, int y, quint16 *arrR, bool *arrRChanged);
    void drawMemoryForRegister(QPainter &painter, int reg, CProcessor *pProc, int x, int y);
    void drawPorts(QPainter &painter, bool okProcessor, CMemoryController* pMemCtl, CMotherboard* pBoard, int x, int y);
    void drawCPUMemoryMap(QPainter &painter, int x, int y, bool okHalt);
    void drawPPUMemoryMap(QPainter &painter, int x, int y, const CMemoryController* pMemCtl);
};

#endif // QDEBUGVIEW_H
