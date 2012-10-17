#include "stdafx.h"
#include <QtGui>
#include "qdebugview.h"
#include "Emulator.h"
#include "emubase/Emubase.h"


//////////////////////////////////////////////////////////////////////


QDebugView::QDebugView(QWidget *parent) :
        QWidget(parent)
{
    m_okDebugProcessor = FALSE;

    QFont font = Common_GetMonospacedFont();
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();
    this->setMinimumSize(cxChar * 55, cyLine * 16 + cyLine / 2);
    this->setMaximumHeight(cyLine * 16 + cyLine / 2);
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
        WORD value = pCPU->GetReg(r);
        m_okDebugCpuRChanged[r] = (m_wDebugCpuR[r] != value);
        m_wDebugCpuR[r] = value;
    }
    WORD pswCPU = pCPU->GetPSW();
    m_okDebugCpuRChanged[8] = (m_wDebugCpuR[8] != pswCPU);
    m_wDebugCpuR[8] = pswCPU;

    CProcessor* pPPU = g_pBoard->GetPPU();
    ASSERT(pPPU != NULL);

    // Get new register values and set change flags
    for (int r = 0; r < 8; r++) {
        WORD value = pPPU->GetReg(r);
        m_okDebugPpuRChanged[r] = (m_wDebugPpuR[r] != value);
        m_wDebugPpuR[r] = value;
    }
    WORD pswPPU = pPPU->GetPSW();
    m_okDebugPpuRChanged[8] = (m_wDebugPpuR[8] != pswPPU);
    m_wDebugPpuR[8] = pswPPU;
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
    WORD* arrR = (m_okDebugProcessor) ? m_wDebugCpuR : m_wDebugPpuR;
    BOOL* arrRChanged = (m_okDebugProcessor) ? m_okDebugCpuRChanged : m_okDebugPpuRChanged;

    //LPCTSTR sProcName = pDebugPU->GetName();
    //painter.drawText(cxChar * 1, 2 * cyLine, sProcName);

    drawProcessor(painter, pDebugPU, cxChar * 2, 1 * cyLine, arrR, arrRChanged);

    // Draw stack
    drawMemoryForRegister(painter, 6, pDebugPU, 35 * cxChar, 1 * cyLine);

    CMemoryController* pDebugMemCtl = pDebugPU->GetMemoryController();
    drawPorts(painter, m_okDebugProcessor, pDebugMemCtl, g_pBoard, 57 * cxChar, 1 * cyLine);
}

void QDebugView::drawProcessor(QPainter &painter, const CProcessor *pProc, int x, int y, WORD *arrR, BOOL *arrRChanged)
{
    QFontMetrics fontmetrics(painter.font());
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();
    QColor colorText = painter.pen().color();

    painter.setPen(QColor(Qt::gray));
    painter.drawRect(x - cxChar, y - cyLine/2, 33 * cxChar, cyLine * 15);

    // Registers
    for (int r = 0; r < 8; r++) {
        painter.setPen(QColor(arrRChanged[r] ? Qt::red : colorText));

        LPCTSTR strRegName = REGISTER_NAME[r];
        painter.drawText(x, y + (1 + r) * cyLine, strRegName);

        WORD value = arrR[r]; //pProc->GetReg(r);
        DrawOctalValue(painter, x + cxChar * 3, y + (1 + r) * cyLine, value);
        DrawHexValue(painter, x + cxChar * 10, y + (1 + r) * cyLine, value);
        DrawBinaryValue(painter, x + cxChar * 15, y + (1 + r) * cyLine, value);
    }
    painter.setPen(colorText);

    // CPC value
    painter.drawText(x, y + 9 * cyLine, _T("PC'"));
    WORD cpc = pProc->GetCPC();
    DrawOctalValue(painter, x + cxChar * 3, y + 9 * cyLine, cpc);
    DrawBinaryValue(painter, x + cxChar * 15, y + 9 * cyLine, cpc);

    // PSW value
    painter.setPen(QColor(arrRChanged[8] ? Qt::red : colorText));
    painter.drawText(x, y + 11 * cyLine, _T("PS"));
    WORD psw = arrR[8]; // pProc->GetPSW();
    DrawOctalValue(painter, x + cxChar * 3, y + 11 * cyLine, psw);
    DrawHexValue(painter, x + cxChar * 10, y + 11 * cyLine, psw);
    painter.drawText(x + cxChar * 15, y + 10 * cyLine, _T("       HP  TNZVC"));
    DrawBinaryValue(painter, x + cxChar * 15, y + 11 * cyLine, psw);

    painter.setPen(colorText);

    // CPSW value
    painter.drawText(x, y + 12 * cyLine, _T("PS'"));
    WORD cpsw = pProc->GetCPSW();
    DrawOctalValue(painter, x + cxChar * 3, y + 12 * cyLine, cpsw);
    DrawBinaryValue(painter, x + cxChar * 15, y + 12 * cyLine, cpsw);

    // Processor mode - HALT or USER
    BOOL okHaltMode = pProc->IsHaltMode();
    painter.drawText(x, y + 14 * cyLine, okHaltMode ? _T("HALT") : _T("USER"));

    // "Stopped" flag
    BOOL okStopped = pProc->IsStopped();
    if (okStopped)
        painter.drawText(x + 6 * cxChar, y + 14 * cyLine, _T("STOP"));
}

void QDebugView::drawMemoryForRegister(QPainter &painter, int reg, CProcessor *pProc, int x, int y)
{
    QFontMetrics fontmetrics(painter.font());
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();
    QColor colorText = painter.pen().color();

    WORD current = pProc->GetReg(reg);
    BOOL okExec = (reg == 7);

    // Читаем из памяти процессора в буфер
    WORD memory[16];
    CMemoryController* pMemCtl = pProc->GetMemoryController();
    for (int idx = 0; idx < 16; idx++) {
        int addrtype;
        memory[idx] = pMemCtl->GetWordView(
                current + idx * 2 - 16, pProc->IsHaltMode(), okExec, &addrtype);
    }

    WORD address = current - 16;
    for (int index = 0; index < 16; index++) {  // Рисуем строки
        // Адрес
        DrawOctalValue(painter, x + 4 * cxChar, y, address);

        // Значение по адресу
        WORD value = memory[index];
//        WORD wChanged = Emulator_GetChangeRamStatus(address);
//        painter.setPen(wChanged != 0 ? Qt::red : colorText);
        DrawOctalValue(painter, x + 12 * cxChar, y, value);
//        painter.setPen(colorText);

        // Текущая позиция
        if (address == current) {
            painter.drawText(x + 2 * cxChar, y, _T(">>"));
            painter.setPen(m_okDebugCpuRChanged[reg] != 0 ? Qt::red : colorText);
            painter.drawText(x, y, REGISTER_NAME[reg]);
            painter.setPen(colorText);
        }

        address += 2;
        y += cyLine;
    }
}

void QDebugView::drawPorts(QPainter &painter, BOOL okProcessor, CMemoryController* pMemCtl, CMotherboard* pBoard, int x, int y)
{
    QFontMetrics fontmetrics(painter.font());
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();

    painter.drawText(x, y, _T("Ports:"));

    if (okProcessor)  // CPU
    {
        WORD value176640 = pMemCtl->GetPortView(0176640);
        DrawOctalValue(painter, x + 0 * cxChar, y + 1 * cyLine, 0176640);
        DrawOctalValue(painter, x + 8 * cxChar, y + 1 * cyLine, value176640);
        WORD value176642 = pMemCtl->GetPortView(0176642);
        DrawOctalValue(painter, x + 0 * cxChar, y + 2 * cyLine, 0176642);
        DrawOctalValue(painter, x + 8 * cxChar, y + 2 * cyLine, value176642);

        //TODO
    }
    else  // PPU
    {
        WORD value177010 = pMemCtl->GetPortView(0177010);
        DrawOctalValue(painter, x + 0 * cxChar, y + 1 * cyLine, 0177010);
        DrawOctalValue(painter, x + 8 * cxChar, y + 1 * cyLine, value177010);
        WORD value177012 = pMemCtl->GetPortView(0177012);
        DrawOctalValue(painter, x + 0 * cxChar, y + 2 * cyLine, 0177012);
        DrawOctalValue(painter, x + 8 * cxChar, y + 2 * cyLine, value177012);
        WORD value177014 = pMemCtl->GetPortView(0177014);
        DrawOctalValue(painter, x + 0 * cxChar, y + 3 * cyLine, 0177014);
        DrawOctalValue(painter, x + 8 * cxChar, y + 3 * cyLine, value177014);
        WORD value177016 = pMemCtl->GetPortView(0177016);
        DrawOctalValue(painter, x + 0 * cxChar, y + 4 * cyLine, 0177016);
        DrawOctalValue(painter, x + 8 * cxChar, y + 4 * cyLine, value177016);
        WORD value177020 = pMemCtl->GetPortView(0177020);
        DrawOctalValue(painter, x + 0 * cxChar, y + 5 * cyLine, 0177020);
        DrawOctalValue(painter, x + 8 * cxChar, y + 5 * cyLine, value177020);
        WORD value177022 = pMemCtl->GetPortView(0177022);
        DrawOctalValue(painter, x + 0 * cxChar, y + 6 * cyLine, 0177022);
        DrawOctalValue(painter, x + 8 * cxChar, y + 6 * cyLine, value177022);
        WORD value177024 = pMemCtl->GetPortView(0177024);
        DrawOctalValue(painter, x + 0 * cxChar, y + 7 * cyLine, 0177024);
        DrawOctalValue(painter, x + 8 * cxChar, y + 7 * cyLine, value177024);
        WORD value177026 = pMemCtl->GetPortView(0177026);
        DrawOctalValue(painter, x + 0 * cxChar, y + 8 * cyLine, 0177026);
        DrawOctalValue(painter, x + 8 * cxChar, y + 8 * cyLine, value177026);
        WORD value177054 = pMemCtl->GetPortView(0177054);
        DrawOctalValue(painter, x + 0 * cxChar, y + 9 * cyLine, 0177054);
        DrawOctalValue(painter, x + 8 * cxChar, y + 9 * cyLine, value177054);
        WORD value177700 = pMemCtl->GetPortView(0177700);
        DrawOctalValue(painter, x + 0 * cxChar, y + 10 * cyLine, 0177700);
        DrawOctalValue(painter, x + 8 * cxChar, y + 10 * cyLine, value177700);
		WORD value177716 = pMemCtl->GetPortView(0177716);
        DrawOctalValue(painter, x + 0 * cxChar, y + 11 * cyLine, 0177716);
        DrawOctalValue(painter, x + 8 * cxChar, y + 11 * cyLine, value177716);

        WORD value177710 = pBoard->GetTimerStateView();
        DrawOctalValue(painter, x + 0 * cxChar, y + 13 * cyLine, 0177710);
        DrawOctalValue(painter, x + 8 * cxChar, y + 13 * cyLine, value177710);
        WORD value177712 = pBoard->GetTimerReloadView();
        DrawOctalValue(painter, x + 0 * cxChar, y + 14 * cyLine, 0177712);
        DrawOctalValue(painter, x + 8 * cxChar, y + 14 * cyLine, value177712);
        WORD value177714 = pBoard->GetTimerValueView();
        DrawOctalValue(painter, x + 0 * cxChar, y + 15 * cyLine, 0177714);
        DrawOctalValue(painter, x + 8 * cxChar, y + 15 * cyLine, value177714);
	}
}

