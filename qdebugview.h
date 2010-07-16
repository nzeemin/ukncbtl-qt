#ifndef QDEBUGVIEW_H
#define QDEBUGVIEW_H

#include <QWidget>
#include "Common.h"

class CProcessor;

class QDebugView : public QWidget
{
    Q_OBJECT
public:
    QDebugView(QWidget *parent = 0);

    void setCurrentProc(bool okProc);
    void updateData();

protected:
    void paintEvent(QPaintEvent *event);

private:
    BOOL m_okDebugProcessor;        // TRUE - CPU, FALSE - PPU
    unsigned short m_wDebugCpuR[9];  // Old register values - R0..R7, PSW
    unsigned short m_wDebugPpuR[9];  // Old register values - R0..R7, PSW
    BOOL m_okDebugCpuRChanged[9];   // Register change flags
    BOOL m_okDebugPpuRChanged[9];   // Register change flags

private:
    void DrawProcessor(QPainter &painter, const CProcessor *pProc, int x, int y, WORD *arrR, BOOL *arrRChanged);
    void DrawMemoryForRegister(QPainter &painter, int reg, CProcessor *pProc, int x, int y);
};

#endif // QDEBUGVIEW_H
