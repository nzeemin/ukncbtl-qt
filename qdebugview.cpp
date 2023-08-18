#include "stdafx.h"
#include <QtGui>
#include <QMenu>
#include <QStyleFactory>
#include <QStyleOptionFocusRect>
#include <QToolBar>
#include "main.h"
#include "mainwindow.h"
#include "qdebugview.h"
#include "Emulator.h"
#include "emubase/Emubase.h"


//////////////////////////////////////////////////////////////////////


QDebugView::QDebugView(QWidget *mainWindow) :
    QWidget()
{
    m_okDebugProcessor = Settings_GetDebugCpuPpu();

    QFont font = Common_GetMonospacedFont();
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();
    int cyHeight = cyLine * 16 + cyLine / 2;
    this->setMinimumSize(36 + cxChar * 33, cyHeight);
    this->setMaximumHeight(cyHeight);

    m_toolbar = new QToolBar(this);
    m_toolbar->setGeometry(0, 0, 36, cyHeight);
    m_toolbar->setOrientation(Qt::Vertical);
    m_toolbar->setIconSize(QSize(24, 24));
    m_toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_toolbar->setFocusPolicy(Qt::NoFocus);
    m_toolbar->setStyle(QStyleFactory::create("windows"));  // fix for macOS to remove gradient background

    int x = 36 + 4;
    int cxProc = cxChar * 33;
    m_procCtrl = new QDebugProcessorCtrl(this);
    m_procCtrl->setGeometry(x, 0, cxProc, cyHeight);
    x += cxProc + 4;
    int cxStack = cxChar * 17 + cxChar / 2;
    m_stackCtrl = new QDebugStackCtrl(this);
    m_stackCtrl->setGeometry(x, 0, cxStack, cyHeight);
    x += cxStack + 4;
    int cxPorts = cxChar * 15;
    m_portsCtrl = new QDebugPortsCtrl(this);
    m_portsCtrl->setGeometry(x, 0, cxPorts, cyHeight);
    x += cxPorts + 4;
    int cxBreaks = cxChar * 9;
    m_breaksCtrl = new QDebugBreakpointsCtrl(this);
    m_breaksCtrl->setGeometry(x, 0, cxBreaks, cyHeight);
    x += cxBreaks + 4;
    int cxMemmap = cxChar * 25;
    m_memmapCtrl = new QDebugMemoryMapCtrl(this);
    m_memmapCtrl->setGeometry(x, 0, cxMemmap, cyHeight);

    QAction* actionCpuPpu = m_toolbar->addAction(QIcon(":/images/iconCpuPpu.svg"), "");
    m_toolbar->addSeparator();
    QAction* actionStepInto = m_toolbar->addAction(QIcon(":/images/iconStepInto.svg"), "");
    QAction* actionStepOver = m_toolbar->addAction(QIcon(":/images/iconStepOver.svg"), "");

    QObject::connect(actionCpuPpu, SIGNAL(triggered()), this, SLOT(switchCpuPpu()));
    QObject::connect(actionStepInto, SIGNAL(triggered()), mainWindow, SLOT(debugStepInto()));
    QObject::connect(actionStepOver, SIGNAL(triggered()), mainWindow, SLOT(debugStepOver()));

    setFocusPolicy(Qt::ClickFocus);
}

void QDebugView::updateWindowText()
{
    CProcessor* pDebugPU = getCurrentProc();
    QString buffer = tr("Debug - %1").arg(pDebugPU->GetName());
    parentWidget()->setWindowTitle(buffer);
}

void QDebugView::setCurrentProc(bool okProc)
{
    m_okDebugProcessor = okProc;
    this->updateData();
    updateWindowText();

    Settings_SetDebugCpuPpu(m_okDebugProcessor);
}

CProcessor* QDebugView::getCurrentProc() const
{
    return (m_okDebugProcessor) ? g_pBoard->GetCPU() : g_pBoard->GetPPU();
}

bool QDebugView::isCpuOrPpu() const
{
    return m_okDebugProcessor;
}

// Update after Run or Step
void QDebugView::updateData()
{
    m_procCtrl->updateData();
    m_stackCtrl->updateData();
    m_portsCtrl->updateData();
    m_breaksCtrl->updateData();
    m_memmapCtrl->updateData();
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
    menu.addAction(m_okDebugProcessor ? tr("Switch to PPU") : tr("Switch to CPU"), this, SLOT(switchCpuPpu()));
    menu.exec(event->globalPos());
}

void QDebugView::switchCpuPpu()
{
    Global_SetCurrentProc(! m_okDebugProcessor);
    Global_UpdateAllViews();
}

void QDebugView::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

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

//////////////////////////////////////////////////////////////////////

QDebugCtrl::QDebugCtrl(QDebugView *debugView)
{
    m_pDebugView = debugView;
    setParent(debugView);
    memset(&m_lastHitTest, 0, sizeof(m_lastHitTest));
}

DebugCtrlHitTest QDebugCtrl::hitTest(int /*x*/, int y)
{
    QFont font = Common_GetMonospacedFont();
    QFontMetrics fontmetrics(font);
    int cyLine = fontmetrics.height();

    DebugCtrlHitTest hit;
    hit.isValid = false;
    hit.address = hit.value = 0;
    hit.line = (y - cyLine / 3) / cyLine;

    m_lastHitTest = hit;
    return hit;
}

void QDebugCtrl::copyAddressOctal()
{
    if (!m_lastHitTest.isValid)
        return;
    CopyWordOctalToClipboard(m_lastHitTest.address);
}

void QDebugCtrl::copyValueOctal()
{
    if (!m_lastHitTest.isValid)
        return;
    CopyWordOctalToClipboard(m_lastHitTest.value);
}

void QDebugCtrl::copyValueHex()
{
    if (!m_lastHitTest.isValid)
        return;
    CopyWordHexToClipboard(m_lastHitTest.value);
}

void QDebugCtrl::copyValueBinary()
{
    if (!m_lastHitTest.isValid)
        return;
    CopyWordBinaryToClipboard(m_lastHitTest.value);
}

void QDebugCtrl::switchCpuPpu()
{
    Global_SetCurrentProc(!isCpuOrPpu());
    Global_UpdateAllViews();
}

void QDebugCtrl::addStandardContextMenuItems(QMenu& menu)
{
    if (!menu.isEmpty())
        menu.addSeparator();
    menu.addAction(isCpuOrPpu() ? tr("Switch to PPU") : tr("Switch to CPU"), this, SLOT(switchCpuPpu()));
}

//////////////////////////////////////////////////////////////////////

QDebugProcessorCtrl::QDebugProcessorCtrl(QDebugView *debugView)
    : QDebugCtrl(debugView)
{
    memset(m_wDebugCpuR, 0, sizeof(m_wDebugCpuR));
    memset(m_wDebugPpuR, 0, sizeof(m_wDebugPpuR));
    memset(m_okDebugCpuRChanged, 0, sizeof(m_okDebugCpuRChanged));
    memset(m_okDebugPpuRChanged, 0, sizeof(m_okDebugPpuRChanged));
}

void QDebugProcessorCtrl::paintEvent(QPaintEvent * /*event*/)
{
    const CProcessor* pProc = getProc();

    quint16* arrR = (isCpuOrPpu()) ? m_wDebugCpuR : m_wDebugPpuR;
    bool* arrRChanged = (isCpuOrPpu()) ? m_okDebugCpuRChanged : m_okDebugPpuRChanged;

    QColor colorBackground = palette().color(QPalette::Base);
    QPainter painter(this);
    painter.fillRect(0, 0, this->width(), this->height(), colorBackground);

    QFont font = Common_GetMonospacedFont();
    painter.setFont(font);
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();
    QColor colorText = palette().color(QPalette::Text);
    QColor colorChanged = Common_GetColorShifted(palette(), COLOR_VALUECHANGED);

    int x = cxChar * 1, y = 0;

    // Registers
    for (int r = 0; r < 8; r++)
    {
        painter.setPen(QColor(arrRChanged[r] ? colorChanged : colorText));

        const char * strRegName = REGISTER_NAME[r];
        painter.drawText(x, y + (1 + r) * cyLine, strRegName);

        quint16 value = arrR[r]; //pProc->GetReg(r);
        DrawOctalValue(painter, x + cxChar * 3, y + (1 + r) * cyLine, value);
        DrawHexValue(painter, x + cxChar * 10, y + (1 + r) * cyLine, value);
        DrawBinaryValue(painter, x + cxChar * 15, y + (1 + r) * cyLine, value);
    }
    painter.setPen(colorText);

    // CPC value
    painter.drawText(x, y + 9 * cyLine, "PC'");
    quint16 cpc = pProc->GetCPC();
    DrawOctalValue(painter, x + cxChar * 3, y + 9 * cyLine, cpc);
    DrawHexValue(painter, x + cxChar * 10, y + 9 * cyLine, cpc);
    DrawBinaryValue(painter, x + cxChar * 15, y + 9 * cyLine, cpc);

    // PSW value
    painter.setPen(QColor(arrRChanged[8] ? colorChanged : colorText));
    painter.drawText(x, y + 11 * cyLine, "PS");
    quint16 psw = arrR[8]; // pProc->GetPSW();
    DrawOctalValue(painter, x + cxChar * 3, y + 11 * cyLine, psw);
    //DrawHexValue(painter, x + cxChar * 10, y + 11 * cyLine, psw);
    painter.drawText(x + cxChar * 15, y + 10 * cyLine, "       HP  TNZVC");
    DrawBinaryValue(painter, x + cxChar * 15, y + 11 * cyLine, psw);

    painter.setPen(colorText);

    // CPSW value
    painter.drawText(x, y + 12 * cyLine, "PS'");
    quint16 cpsw = pProc->GetCPSW();
    DrawOctalValue(painter, x + cxChar * 3, y + 12 * cyLine, cpsw);
    DrawBinaryValue(painter, x + cxChar * 15, y + 12 * cyLine, cpsw);

    // Processor mode - HALT or USER
    bool okHaltMode = pProc->IsHaltMode();
    painter.drawText(x, y + 14 * cyLine, okHaltMode ? "HALT" : "USER");

    // "Stopped" flag
    bool okStopped = pProc->IsStopped();
    if (okStopped)
        painter.drawText(x + 6 * cxChar, y + 14 * cyLine, "STOP");
}

void QDebugProcessorCtrl::updateData()
{
    const CProcessor* pCPU = g_pBoard->GetCPU();
    ASSERT(pCPU != nullptr);

    // Get new register values and set change flags
    for (int r = 0; r < 8; r++)
    {
        quint16 value = pCPU->GetReg(r);
        m_okDebugCpuRChanged[r] = (m_wDebugCpuR[r] != value);
        m_wDebugCpuR[r] = value;
    }
    quint16 pswCPU = pCPU->GetPSW();
    m_okDebugCpuRChanged[8] = (m_wDebugCpuR[8] != pswCPU);
    m_wDebugCpuR[8] = pswCPU;

    CProcessor* pPPU = g_pBoard->GetPPU();
    ASSERT(pPPU != nullptr);

    // Get new register values and set change flags
    for (int r = 0; r < 8; r++)
    {
        quint16 value = pPPU->GetReg(r);
        m_okDebugPpuRChanged[r] = (m_wDebugPpuR[r] != value);
        m_wDebugPpuR[r] = value;
    }
    quint16 pswPPU = pPPU->GetPSW();
    m_okDebugPpuRChanged[8] = (m_wDebugPpuR[8] != pswPPU);
    m_wDebugPpuR[8] = pswPPU;
}

DebugCtrlHitTest QDebugProcessorCtrl::hitTest(int x, int y)
{
    DebugCtrlHitTest hit = QDebugCtrl::hitTest(x, y);
    if (hit.line < 0 || hit.line == 9 || hit.line > 11)
        return hit;  // Invalid line number
    hit.isValid = true;

    const CProcessor* pProc = getProc();
    if (hit.line < 8)
        hit.value = pProc->GetReg(hit.line);
    else if (hit.line == 8)
        hit.value = pProc->GetCPC();
    else if (hit.line == 10)
        hit.value = pProc->GetPSW();
    else if (hit.line == 11)
        hit.value = pProc->GetCPSW();

    return hit;
}

void QDebugProcessorCtrl::contextMenuEvent(QContextMenuEvent *event)
{
    DebugCtrlHitTest hit = hitTest(event->x(), event->y());
    m_lastHitTest = hit;

    QMenu menu(this);
    if (hit.isValid)
    {
        char buffer[7], bufferHex[5], bufferBin[17];
        PrintOctalValue(buffer, hit.value);
        PrintHexValue(bufferHex, hit.value);
        PrintBinaryValue(bufferBin, hit.value);
        menu.addAction(tr("Copy Value %1").arg(buffer), this, SLOT(copyValueOctal()));
        if (hit.line < 10)
            menu.addAction(tr("Copy Value %1").arg(bufferHex), this, SLOT(copyValueHex()));
        menu.addAction(tr("Copy Value %1").arg(bufferBin), this, SLOT(copyValueBinary()));
    }
    addStandardContextMenuItems(menu);
    menu.exec(event->globalPos());
}

//////////////////////////////////////////////////////////////////////

QDebugStackCtrl::QDebugStackCtrl(QDebugView *debugView)
    : QDebugCtrl(debugView)
{
    m_wDebugCpuR6Old = m_wDebugPpuR6Old = 0;
}

void QDebugStackCtrl::paintEvent(QPaintEvent * /*event*/)
{
    QColor colorBackground = palette().color(QPalette::Base);
    QPainter painter(this);
    painter.fillRect(0, 0, this->width(), this->height(), colorBackground);

    QFont font = Common_GetMonospacedFont();
    painter.setFont(font);
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();
    QColor colorText = palette().color(QPalette::Text);
    QColor colorChanged = Common_GetColorShifted(palette(), COLOR_VALUECHANGED);
    QColor colorPrev = Common_GetColorShifted(palette(), COLOR_PREVIOUS);

    int x = cxChar / 2, y = cyLine;

    CProcessor* pProc = getProc();
    int reg = 6;
    quint16 oldValue = (isCpuOrPpu()) ? m_wDebugCpuR6Old : m_wDebugPpuR6Old;

    quint16 current = pProc->GetReg(reg);
    quint16 previous = oldValue;
    bool okExec = false; //(reg == 7);

    // Reading from memory into the buffer
    quint16 memory[16];
    int addrtype[16];
    CMemoryController* pMemCtl = pProc->GetMemoryController();
    for (int idx = 0; idx < 16; idx++)
    {
        memory[idx] = pMemCtl->GetWordView(
                current + idx * 2 - 16, pProc->IsHaltMode(), okExec, addrtype + idx);
    }

    quint16 address = current - 16;
    for (int index = 0; index < 16; index++)    // Draw strings
    {
        // Address
        painter.setPen(colorText);
        DrawOctalValue(painter, x + 3 * cxChar, y, address);

        // Value at the address
        quint16 value = memory[index];
        quint16 wChanged = Emulator_GetChangeRamStatus(addrtype[index], address);
        painter.setPen(wChanged != 0 ? colorChanged : colorText);
        DrawOctalValue(painter, x + 10 * cxChar, y, value);

        // Current position
        if (address == current)
        {
            painter.setPen(colorText);
            painter.drawText(x + 2 * cxChar, y, ">");
            painter.setPen(current != previous ? colorChanged : colorText);
            painter.drawText(x, y, REGISTER_NAME[reg]);
        }
        else if (address == previous)
        {
            painter.setPen(colorPrev);
            painter.drawText(x + 2 * cxChar, y, ">");
        }

        address += 2;
        y += cyLine;
    }

    painter.setPen(colorText);
}

void QDebugStackCtrl::updateData()
{
    m_wDebugCpuR6Old = g_pBoard->GetCPU()->GetSP();

    m_wDebugPpuR6Old = g_pBoard->GetPPU()->GetSP();
}

DebugCtrlHitTest QDebugStackCtrl::hitTest(int x, int y)
{
    DebugCtrlHitTest hit = QDebugCtrl::hitTest(x, y);
    if (hit.line < 0 || hit.line > 15)
        return hit;  // Invalid line number
    hit.isValid = true;

    const CProcessor* pProc = getProc();
    const CMemoryController* pMemCtl = pProc->GetMemoryControllerConst();

    hit.address = pProc->GetSP() - 16 + hit.line * 2;

    int addrtype;
    hit.value = pMemCtl->GetWordView(hit.address, pProc->IsHaltMode(), false, &addrtype);

    return hit;
}

void QDebugStackCtrl::contextMenuEvent(QContextMenuEvent *event)
{
    DebugCtrlHitTest hit = hitTest(event->x(), event->y());
    if (!hit.isValid)
        return;
    m_lastHitTest = hit;

    char bufaddr[7];
    PrintOctalValue(bufaddr, hit.address);
    char bufval[7];
    PrintOctalValue(bufval, hit.value);

    QMenu menu(this);
    menu.addAction(tr("Copy Address %1").arg(bufaddr), this, SLOT(copyAddressOctal()));
    menu.addAction(tr("Copy Value %1").arg(bufval), this, SLOT(copyValueOctal()));
    addStandardContextMenuItems(menu);
    menu.exec(event->globalPos());
}

//////////////////////////////////////////////////////////////////////

QDebugPortsCtrl::QDebugPortsCtrl(QDebugView *debugView)
    : QDebugCtrl(debugView)
{
}

void QDebugPortsCtrl::paintEvent(QPaintEvent * /*event*/)
{
    QColor colorBackground = palette().color(QPalette::Base);
    QPainter painter(this);
    painter.fillRect(0, 0, this->width(), this->height(), colorBackground);

    QFont font = Common_GetMonospacedFont();
    painter.setFont(font);
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();

    int x = cxChar, y = cyLine;

    painter.drawText(x, y, tr("Ports"));

    CMemoryController* pMemCtl = getProc()->GetMemoryController();
    if (isCpuOrPpu())  // CPU
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

        quint16 value177710 = g_pBoard->GetTimerStateView();
        DrawOctalValue(painter, x + 0 * cxChar, y + 13 * cyLine, 0177710);
        DrawOctalValue(painter, x + 7 * cxChar, y + 13 * cyLine, value177710);
        quint16 value177712 = g_pBoard->GetTimerReloadView();
        DrawOctalValue(painter, x + 0 * cxChar, y + 14 * cyLine, 0177712);
        DrawOctalValue(painter, x + 7 * cxChar, y + 14 * cyLine, value177712);
        quint16 value177714 = g_pBoard->GetTimerValueView();
        DrawOctalValue(painter, x + 0 * cxChar, y + 15 * cyLine, 0177714);
        DrawOctalValue(painter, x + 7 * cxChar, y + 15 * cyLine, value177714);
    }
}

//////////////////////////////////////////////////////////////////////

QDebugBreakpointsCtrl::QDebugBreakpointsCtrl(QDebugView *debugView)
    : QDebugCtrl(debugView)
{
}

void QDebugBreakpointsCtrl::paintEvent(QPaintEvent * /*event*/)
{
    QColor colorBackground = palette().color(QPalette::Base);
    QPainter painter(this);
    painter.fillRect(0, 0, this->width(), this->height(), colorBackground);

    QFont font = Common_GetMonospacedFont();
    painter.setFont(font);
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();

    int x = cxChar / 2, y = cyLine;

    painter.drawText(x, y, tr("Breakpts"));

    const quint16* pbps = isCpuOrPpu() ? Emulator_GetCPUBreakpointList() : Emulator_GetPPUBreakpointList();
    if (*pbps == 0177777)
        return;

    y += cyLine;
    while (*pbps != 0177777)
    {
        DrawOctalValue(painter, x + cxChar, y, *pbps);
        y += cyLine;
        pbps++;
    }
}

void QDebugBreakpointsCtrl::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(tr("Remove All Breakpoints"), this, SLOT(removeAllBreakpoints()));
    addStandardContextMenuItems(menu);
    menu.exec(event->globalPos());
}

void QDebugBreakpointsCtrl::removeAllBreakpoints()
{
    Global_getMainWindow()->debugRemoveAllBreakpoints();
}

//////////////////////////////////////////////////////////////////////

QDebugMemoryMapCtrl::QDebugMemoryMapCtrl(QDebugView *debugView)
    : QDebugCtrl(debugView)
{
}

void QDebugMemoryMapCtrl::paintEvent(QPaintEvent * /*event*/)
{
    QColor colorBackground = palette().color(QPalette::Base);
    QPainter painter(this);
    painter.fillRect(0, 0, this->width(), this->height(), colorBackground);

    QFont font = Common_GetMonospacedFont();
    painter.setFont(font);
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();

    int x = cxChar, y = 0;
    int x1 = x + cxChar * 7 - cxChar / 2;
    int y1 = y + cxChar / 2;
    int x2 = x1 + cxChar * 13;
    int y2 = y1 + cyLine * 16;
    int xtype = x1 + cxChar * 3;
    int ybase = y + cyLine * 16;
    painter.drawRect(x1, y1, x2 - x1, y2 - y1);
    painter.drawText(x, ybase + 2, "000000");

    if (isCpuOrPpu())
    {
        for (int i = 1; i < 8; i++)
        {
            int ycur = y2 - cyLine * i * 2;
            if (i < 7)
                painter.drawLine(x1, ycur, x1 + 8, ycur);
            else
                painter.drawLine(x1, ycur, x2, ycur);
            quint16 addr = (quint16)i * 020000;
            DrawOctalValue(painter, x, y2 - cyLine * i * 2 + cyLine / 3, addr);
        }

        painter.drawText(xtype, ybase - cyLine * 6 - cyLine / 3, "RAM12");

        int ytop = ybase - cyLine * 14 - cyLine / 3;
        if (getProc()->IsHaltMode())
            painter.drawText(xtype, ytop, "RAM12");
        else
            painter.drawText(xtype, ytop, "I/O");
    }
    else
    {
        const CMemoryController *pMemCtl = getProc()->GetMemoryController();
        quint16 value177054 = pMemCtl->GetPortView(0177054);

        for (int i = 1; i < 8; i++)
        {
            int ycur = y2 - cyLine * i * 2;
            if (i < 4)
                painter.drawLine(x1, ycur, x1 + 8, ycur);
            else
                painter.drawLine(x1, ycur, x2, ycur);
            quint16 addr = (quint16)i * 020000;
            DrawOctalValue(painter, x, y2 - cyLine * i * 2 + cyLine / 3, addr);
        }

        int yp = y1 + cyLine / 4;
        painter.drawLine(x1, yp, x2, yp);

        painter.drawText(x, ybase - cyLine * 15, "177000");
        painter.drawText(xtype, ybase - cyLine * 3 - cyLine / 2, "RAM0");

        // 100000-117777 - Window block 0
        int yb0 = ybase - cyLine * 8 - cyLine / 3;
        if ((value177054 & 16) != 0)  // Port 177054 bit 4 set => RAM selected
            painter.drawText(xtype, yb0, "RAM0");
        else if ((value177054 & 1) != 0)  // ROM selected
            painter.drawText(xtype, yb0, "ROM");
        else if ((value177054 & 14) != 0)  // ROM cartridge selected
        {
            int slot = ((value177054 & 8) == 0) ? 1 : 2;
            int bank = (value177054 & 6) >> 1;
            QString carttext = tr("Cart %1/%2").arg(slot).arg(bank);
            painter.drawText(xtype, yb0, carttext);
        }

        // 120000-137777 - Window block 1
        int yb1 = ybase - cyLine * 10 - cyLine / 3;
        if ((value177054 & 32) != 0)  // Port 177054 bit 5 set => RAM selected
            painter.drawText(xtype, yb1, "RAM0");
        else
            painter.drawText(xtype, yb1, "ROM");

        // 140000-157777 - Window block 2
        int yb2 = ybase - cyLine * 12 - cyLine / 3;
        if ((value177054 & 64) != 0)  // Port 177054 bit 6 set => RAM selected
            painter.drawText(xtype, yb2, "RAM0");
        else
            painter.drawText(xtype, yb2, "ROM");

        // 160000-176777 - Window block 3
        int yb3 = ybase - cyLine * 14 - cyLine / 3;
        if ((value177054 & 128) != 0)  // Port 177054 bit 7 set => RAM selected
            painter.drawText(xtype, yb3, "RAM0");
        else
            painter.drawText(xtype, yb3, "ROM");
    }

    quint16 sp = getProc()->GetSP();
    int ysp = y2 - ((y2 - y1) * sp / 65536);
    painter.drawLine(x2, ysp, x2 + cxChar, ysp);
    painter.drawText(x2 + cxChar, ysp + cyLine / 4, "SP");

    quint16 pc = getProc()->GetPC();
    int ypc = y2 - ((y2 - y1) * pc / 65536);
    painter.drawLine(x2, ypc, x2 + cxChar, ypc);
    painter.drawText(x2 + cxChar, ypc + cyLine / 4, "PC");
}

//////////////////////////////////////////////////////////////////////
