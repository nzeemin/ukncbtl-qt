#include "stdafx.h"
#include <QtGui>
#include <QMenu>
#include <QStyleOptionFocusRect>
#include "main.h"
#include "qdebugview.h"
#include "Emulator.h"
#include "emubase/Emubase.h"


//////////////////////////////////////////////////////////////////////


QDebugView::QDebugView(QWidget *parent) :
        QWidget(parent)
{
    m_okDebugProcessor = false;

    QFont font = Common_GetMonospacedFont();
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();
    this->setMinimumSize(cxChar * 55, cyLine * 16 + cyLine / 2);
    this->setMaximumHeight(cyLine * 16 + cyLine / 2);

    setFocusPolicy(Qt::ClickFocus);
}

void QDebugView::updateWindowText()
{
    CProcessor* pDebugPU = m_okDebugProcessor ? g_pBoard->GetCPU() : g_pBoard->GetPPU();
    QString buffer = QString(_T("Debug - %1")).arg(pDebugPU->GetName());
    parentWidget()->setWindowTitle(buffer);
}

void QDebugView::setCurrentProc(bool okProc)
{
    m_okDebugProcessor = okProc;
    this->updateData();
    updateWindowText();
}

void QDebugView::updateData()
{
    CProcessor* pCPU = g_pBoard->GetCPU();
    ASSERT(pCPU != NULL);

    // Get new register values and set change flags
    for (int r = 0; r < 8; r++) {
        quint16 value = pCPU->GetReg(r);
        m_okDebugCpuRChanged[r] = (m_wDebugCpuR[r] != value);
        m_wDebugCpuR[r] = value;
    }
    quint16 pswCPU = pCPU->GetPSW();
    m_okDebugCpuRChanged[8] = (m_wDebugCpuR[8] != pswCPU);
    m_wDebugCpuR[8] = pswCPU;

    CProcessor* pPPU = g_pBoard->GetPPU();
    ASSERT(pPPU != NULL);

    // Get new register values and set change flags
    for (int r = 0; r < 8; r++) {
        quint16 value = pPPU->GetReg(r);
        m_okDebugPpuRChanged[r] = (m_wDebugPpuR[r] != value);
        m_wDebugPpuR[r] = value;
    }
    quint16 pswPPU = pPPU->GetPSW();
    m_okDebugPpuRChanged[8] = (m_wDebugPpuR[8] != pswPPU);
    m_wDebugPpuR[8] = pswPPU;
}

void QDebugView::focusInEvent(QFocusEvent *)
{
    repaint();  // Need to draw focus rect
}
void QDebugView::focusOutEvent(QFocusEvent *)
{
    repaint();  // Need to draw focus rect
}

void QDebugView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(m_okDebugProcessor ? "Switch to PPU" : "Switch to CPU", this, SLOT(switchCpuPpu()));
    menu.exec(event->globalPos());
}

void QDebugView::switchCpuPpu()
{
    Global_SetCurrentProc(! m_okDebugProcessor);
    Global_UpdateAllViews();
}

void QDebugView::paintEvent(QPaintEvent * /*event*/)
{
    if (g_pBoard == NULL) return;

    QPainter painter(this);
    painter.fillRect(0,0, this->width(), this->height(), Qt::white);

    QFont font = Common_GetMonospacedFont();
    painter.setFont(font);
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();

    CProcessor* pDebugPU = (m_okDebugProcessor) ? g_pBoard->GetCPU() : g_pBoard->GetPPU();
    ASSERT(pDebugPU != NULL);
    quint16* arrR = (m_okDebugProcessor) ? m_wDebugCpuR : m_wDebugPpuR;
    bool* arrRChanged = (m_okDebugProcessor) ? m_okDebugCpuRChanged : m_okDebugPpuRChanged;

    //LPCTSTR sProcName = pDebugPU->GetName();
    //painter.drawText(cxChar * 1, 2 * cyLine, sProcName);

    drawProcessor(painter, pDebugPU, cxChar * 2, 1 * cyLine, arrR, arrRChanged);

    // Draw stack
    drawMemoryForRegister(painter, 6, pDebugPU, 35 * cxChar, 1 * cyLine);

    CMemoryController* pDebugMemCtl = pDebugPU->GetMemoryController();
    drawPorts(painter, m_okDebugProcessor, pDebugMemCtl, g_pBoard, 54 * cxChar, 1 * cyLine);

    if (m_okDebugProcessor)
        drawCPUMemoryMap(painter, 70 * cxChar, 0 * cyLine, pDebugPU->IsHaltMode());
    else
        drawPPUMemoryMap(painter, 70 * cxChar, 0 * cyLine, pDebugMemCtl);

    // Draw focus rect
    if (hasFocus())
    {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.state |= QStyle::State_KeyboardFocusChange;
        option.backgroundColor = QColor(Qt::gray);
        option.rect = this->rect();
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
    }
}

void QDebugView::drawProcessor(QPainter &painter, const CProcessor *pProc, int x, int y, quint16 *arrR, bool *arrRChanged)
{
    QFontMetrics fontmetrics(painter.font());
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();
    QColor colorText = painter.pen().color();

    painter.setPen(QColor(Qt::gray));
    painter.drawRect(x - cxChar, y - cyLine/2, 33 * cxChar, cyLine * 15 + cyLine/2);

    // Registers
    for (int r = 0; r < 8; r++) {
        painter.setPen(QColor(arrRChanged[r] ? Qt::red : colorText));

        LPCTSTR strRegName = REGISTER_NAME[r];
        painter.drawText(x, y + (1 + r) * cyLine, strRegName);

        quint16 value = arrR[r]; //pProc->GetReg(r);
        DrawOctalValue(painter, x + cxChar * 3, y + (1 + r) * cyLine, value);
        DrawHexValue(painter, x + cxChar * 10, y + (1 + r) * cyLine, value);
        DrawBinaryValue(painter, x + cxChar * 15, y + (1 + r) * cyLine, value);
    }
    painter.setPen(colorText);

    // CPC value
    painter.drawText(x, y + 9 * cyLine, _T("PC'"));
    quint16 cpc = pProc->GetCPC();
    DrawOctalValue(painter, x + cxChar * 3, y + 9 * cyLine, cpc);
    DrawBinaryValue(painter, x + cxChar * 15, y + 9 * cyLine, cpc);

    // PSW value
    painter.setPen(QColor(arrRChanged[8] ? Qt::red : colorText));
    painter.drawText(x, y + 11 * cyLine, _T("PS"));
    quint16 psw = arrR[8]; // pProc->GetPSW();
    DrawOctalValue(painter, x + cxChar * 3, y + 11 * cyLine, psw);
    DrawHexValue(painter, x + cxChar * 10, y + 11 * cyLine, psw);
    painter.drawText(x + cxChar * 15, y + 10 * cyLine, _T("       HP  TNZVC"));
    DrawBinaryValue(painter, x + cxChar * 15, y + 11 * cyLine, psw);

    painter.setPen(colorText);

    // CPSW value
    painter.drawText(x, y + 12 * cyLine, _T("PS'"));
    quint16 cpsw = pProc->GetCPSW();
    DrawOctalValue(painter, x + cxChar * 3, y + 12 * cyLine, cpsw);
    DrawBinaryValue(painter, x + cxChar * 15, y + 12 * cyLine, cpsw);

    // Processor mode - HALT or USER
    bool okHaltMode = pProc->IsHaltMode();
    painter.drawText(x, y + 14 * cyLine, okHaltMode ? _T("HALT") : _T("USER"));

    // "Stopped" flag
    bool okStopped = pProc->IsStopped();
    if (okStopped)
        painter.drawText(x + 6 * cxChar, y + 14 * cyLine, _T("STOP"));
}

void QDebugView::drawMemoryForRegister(QPainter &painter, int reg, CProcessor *pProc, int x, int y)
{
    QFontMetrics fontmetrics(painter.font());
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();
    QColor colorText = painter.pen().color();

    quint16 current = pProc->GetReg(reg);
    bool okExec = (reg == 7);

    // ������ �� ������ ���������� � �����
    quint16 memory[16];
    int addrtype[16];
    CMemoryController* pMemCtl = pProc->GetMemoryController();
    for (int idx = 0; idx < 16; idx++) {
        memory[idx] = pMemCtl->GetWordView(
                current + idx * 2 - 16, pProc->IsHaltMode(), okExec, addrtype + idx);
    }

    quint16 address = current - 16;
    for (int index = 0; index < 16; index++) {  // ������ ������
        // �����
        DrawOctalValue(painter, x + 3 * cxChar, y, address);

        // �������� �� ������
        quint16 value = memory[index];
        quint16 wChanged = Emulator_GetChangeRamStatus(addrtype[index], address);
        painter.setPen(wChanged != 0 ? Qt::red : colorText);
        DrawOctalValue(painter, x + 10 * cxChar, y, value);
        painter.setPen(colorText);

        // ������� �������
        if (address == current) {
            painter.drawText(x + 2 * cxChar, y, ">");
            painter.setPen(m_okDebugCpuRChanged[reg] != 0 ? Qt::red : colorText);
            painter.drawText(x, y, REGISTER_NAME[reg]);
            painter.setPen(colorText);
        }

        address += 2;
        y += cyLine;
    }
}

void QDebugView::drawPorts(QPainter &painter, bool okProcessor, CMemoryController* pMemCtl, CMotherboard* pBoard, int x, int y)
{
    QFontMetrics fontmetrics(painter.font());
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();

    painter.drawText(x, y, _T("Ports:"));

    if (okProcessor)  // CPU
    {
        quint16 value176640 = pMemCtl->GetPortView(0176640);
        DrawOctalValue(painter, x + 0 * cxChar, y + 1 * cyLine, 0176640);
        DrawOctalValue(painter, x + 7 * cxChar, y + 1 * cyLine, value176640);
        quint16 value176642 = pMemCtl->GetPortView(0176642);
        DrawOctalValue(painter, x + 0 * cxChar, y + 2 * cyLine, 0176642);
        DrawOctalValue(painter, x + 7 * cxChar, y + 2 * cyLine, value176642);

        //TODO
    }
    else  // PPU
    {
        quint16 value177010 = pMemCtl->GetPortView(0177010);
        DrawOctalValue(painter, x + 0 * cxChar, y + 1 * cyLine, 0177010);
        DrawOctalValue(painter, x + 7 * cxChar, y + 1 * cyLine, value177010);
        quint16 value177012 = pMemCtl->GetPortView(0177012);
        DrawOctalValue(painter, x + 0 * cxChar, y + 2 * cyLine, 0177012);
        DrawOctalValue(painter, x + 7 * cxChar, y + 2 * cyLine, value177012);
        quint16 value177014 = pMemCtl->GetPortView(0177014);
        DrawOctalValue(painter, x + 0 * cxChar, y + 3 * cyLine, 0177014);
        DrawOctalValue(painter, x + 7 * cxChar, y + 3 * cyLine, value177014);
        quint16 value177016 = pMemCtl->GetPortView(0177016);
        DrawOctalValue(painter, x + 0 * cxChar, y + 4 * cyLine, 0177016);
        DrawOctalValue(painter, x + 7 * cxChar, y + 4 * cyLine, value177016);
        quint16 value177020 = pMemCtl->GetPortView(0177020);
        DrawOctalValue(painter, x + 0 * cxChar, y + 5 * cyLine, 0177020);
        DrawOctalValue(painter, x + 7 * cxChar, y + 5 * cyLine, value177020);
        quint16 value177022 = pMemCtl->GetPortView(0177022);
        DrawOctalValue(painter, x + 0 * cxChar, y + 6 * cyLine, 0177022);
        DrawOctalValue(painter, x + 7 * cxChar, y + 6 * cyLine, value177022);
        quint16 value177024 = pMemCtl->GetPortView(0177024);
        DrawOctalValue(painter, x + 0 * cxChar, y + 7 * cyLine, 0177024);
        DrawOctalValue(painter, x + 7 * cxChar, y + 7 * cyLine, value177024);
        quint16 value177026 = pMemCtl->GetPortView(0177026);
        DrawOctalValue(painter, x + 0 * cxChar, y + 8 * cyLine, 0177026);
        DrawOctalValue(painter, x + 7 * cxChar, y + 8 * cyLine, value177026);
        quint16 value177054 = pMemCtl->GetPortView(0177054);
        DrawOctalValue(painter, x + 0 * cxChar, y + 9 * cyLine, 0177054);
        DrawOctalValue(painter, x + 7 * cxChar, y + 9 * cyLine, value177054);
        quint16 value177700 = pMemCtl->GetPortView(0177700);
        DrawOctalValue(painter, x + 0 * cxChar, y + 10 * cyLine, 0177700);
        DrawOctalValue(painter, x + 7 * cxChar, y + 10 * cyLine, value177700);
        quint16 value177716 = pMemCtl->GetPortView(0177716);
        DrawOctalValue(painter, x + 0 * cxChar, y + 11 * cyLine, 0177716);
        DrawOctalValue(painter, x + 7 * cxChar, y + 11 * cyLine, value177716);

        quint16 value177710 = pBoard->GetTimerStateView();
        DrawOctalValue(painter, x + 0 * cxChar, y + 13 * cyLine, 0177710);
        DrawOctalValue(painter, x + 7 * cxChar, y + 13 * cyLine, value177710);
        quint16 value177712 = pBoard->GetTimerReloadView();
        DrawOctalValue(painter, x + 0 * cxChar, y + 14 * cyLine, 0177712);
        DrawOctalValue(painter, x + 7 * cxChar, y + 14 * cyLine, value177712);
        quint16 value177714 = pBoard->GetTimerValueView();
        DrawOctalValue(painter, x + 0 * cxChar, y + 15 * cyLine, 0177714);
        DrawOctalValue(painter, x + 7 * cxChar, y + 15 * cyLine, value177714);
	}
}

void QDebugView::drawCPUMemoryMap(QPainter &painter, int x, int y, bool okHalt)
{
    QFontMetrics fontmetrics(painter.font());
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();

    int x1 = x + cxChar * 7;
    int y1 = y + cxChar / 2;
    int x2 = x1 + cxChar * 14;
    int y2 = y1 + cyLine * 16;
    int xtype = x1 + cxChar * 3;
    int ybase = y + cyLine * 16;

    painter.drawRect(x1, y1, x2 - x1, y2 - y1);

    painter.drawText(x, ybase + 2, "000000");

    for (int i = 1; i < 8; i++)
    {
        int ycur = y2 - cyLine * i * 2;
        if (i < 7)
            painter.drawLine(x1, ycur, x1 + 8, ycur);
        else
            painter.drawLine(x1, ycur, x2, ycur);
        quint16 addr = (quint16)i * 020000;
        DrawOctalValue(painter, x, y2 - cyLine * i * 2 + cyLine/3, addr);
    }

    painter.drawText(xtype, ybase - cyLine * 6 - cyLine/3, "RAM12");

    int ytop = ybase - cyLine * 14 - cyLine/3;
    if (okHalt)
        painter.drawText(xtype, ytop, "RAM12");
    else
        painter.drawText(xtype, ytop, "I/O");
}

void QDebugView::drawPPUMemoryMap(QPainter &painter, int x, int y, const CMemoryController *pMemCtl)
{
    QFontMetrics fontmetrics(painter.font());
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();

    quint16 value177054 = pMemCtl->GetPortView(0177054);

    int x1 = x + cxChar * 7;
    int y1 = y + cxChar / 2;
    int x2 = x1 + cxChar * 14;
    int y2 = y1 + cyLine * 16;
    int xtype = x1 + cxChar * 3;
    int ybase = y + cyLine * 16;

    painter.drawRect(x1, y1, x2 - x1, y2 - y1);

    painter.drawText(x, ybase + 2, "000000");

    for (int i = 1; i < 8; i++)
    {
        int ycur = y2 - cyLine * i * 2;
        if (i < 4)
            painter.drawLine(x1, ycur, x1 + 8, ycur);
        else
            painter.drawLine(x1, ycur, x2, ycur);
        quint16 addr = (quint16)i * 020000;
        DrawOctalValue(painter, x, y2 - cyLine * i * 2 + cyLine/3, addr);
    }

    int yp = y1 + cyLine / 4;
    painter.drawLine(x1, yp, x2, yp);

    painter.drawText(x, ybase - cyLine * 15, "177000");
    painter.drawText(xtype, ybase - cyLine * 3 - cyLine/2, "RAM0");

    // 100000-117777 - Window block 0
    int yb0 = ybase - cyLine * 8 - cyLine/3;
    if ((value177054 & 16) != 0)  // Port 177054 bit 4 set => RAM selected
        painter.drawText(xtype, yb0, "RAM0");
    else if ((value177054 & 1) != 0)  // ROM selected
        painter.drawText(xtype, yb0, "ROM");
    else if ((value177054 & 14) != 0)  // ROM cartridge selected
    {
        int slot = ((value177054 & 8) == 0) ? 1 : 2;
        int bank = (value177054 & 6) >> 1;
        TCHAR buffer[10];
        wsprintf(buffer, _T("Cart %d/%d"), slot, bank);
        painter.drawText(xtype, yb0, buffer);
    }

    // 120000-137777 - Window block 1
    int yb1 = ybase - cyLine * 10 - cyLine/3;
    if ((value177054 & 32) != 0)  // Port 177054 bit 5 set => RAM selected
        painter.drawText(xtype, yb1, "RAM0");
    else
        painter.drawText(xtype, yb1, "ROM");

    // 140000-157777 - Window block 2
    int yb2 = ybase - cyLine * 12 - cyLine/3;
    if ((value177054 & 64) != 0)  // Port 177054 bit 6 set => RAM selected
        painter.drawText(xtype, yb2, "RAM0");
    else
        painter.drawText(xtype, yb2, "ROM");

    // 160000-176777 - Window block 3
    int yb3 = ybase - cyLine * 14 - cyLine/3;
    if ((value177054 & 128) != 0)  // Port 177054 bit 7 set => RAM selected
        painter.drawText(xtype, yb3, "RAM0");
    else
        painter.drawText(xtype, yb3, "ROM");
}
