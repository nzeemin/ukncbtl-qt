#include "stdafx.h"
#include <QtGui>
#include <QMenu>
#include <QPainter>
#include <QScrollBar>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleOptionFocusRect>
#include <QToolBar>
#include "main.h"
#include "qmemoryview.h"
#include "Emulator.h"
#include "emubase/Emubase.h"
#include "qdialogs.h"


enum MemoryViewMode
{
    MEMMODE_RAM0 = 0,  // RAM plane 0
    MEMMODE_RAM1 = 1,  // RAM plane 1
    MEMMODE_RAM2 = 2,  // RAM plane 2
    MEMMODE_ROM  = 3,  // ROM
    MEMMODE_CPU  = 4,  // CPU memory
    MEMMODE_PPU  = 5,  // PPU memory
    MEMMODE_LAST = 5   // Last mode number
};

static const char * MemoryView_ModeNames[] =
{
    "RAM0", "RAM1", "RAM2", "ROM", "CPU", "PPU"
};


QMemoryView::QMemoryView()
{
    m_Mode = Settings_GetDebugMemoryMode();
    if (m_Mode > MEMMODE_LAST) m_Mode = MEMMODE_LAST;
    m_ByteMode = Settings_GetDebugMemoryByte();
    m_wBaseAddress = Settings_GetDebugMemoryAddress();
    m_cyLineMemory = 0;
    m_nPageSize = 0;

    QFont font = Common_GetMonospacedFont();
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();

    m_cyLine = cyLine;

    this->setFont(font);
    this->setMinimumSize(cxChar * 68, cyLine * 9 + cyLine / 2);

    m_scrollbar = new QScrollBar(Qt::Vertical, this);
    m_scrollbar->setRange(0, 65536 - 16);
    m_scrollbar->setSingleStep(16);
    QObject::connect(m_scrollbar, SIGNAL(valueChanged(int)), this, SLOT(scrollValueChanged()));

    m_toolbar = new QToolBar(this);
    m_toolbar->setGeometry(4, 4, 28, 2000);
    m_toolbar->setOrientation(Qt::Vertical);
    m_toolbar->setIconSize(QSize(16, 16));
    m_toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_toolbar->setFocusPolicy(Qt::NoFocus);
    m_toolbar->setStyle(QStyleFactory::create("windows"));  // fix for macOS to remove gradient background

    QAction* actionGotoAddr = m_toolbar->addAction(QIcon(":/images/iconEditAddress.png"), "");
    m_toolbar->addSeparator();
    QAction* actionWordByte = m_toolbar->addAction(QIcon(":/images/iconWordByte.png"), "");

    QObject::connect(actionGotoAddr, SIGNAL(triggered()), this, SLOT(gotoAddress()));
    QObject::connect(actionWordByte, SIGNAL(triggered()), this, SLOT(changeWordByteMode()));

    setFocusPolicy(Qt::ClickFocus);
}

QMemoryView::~QMemoryView()
{
    delete m_scrollbar;
}

void QMemoryView::updateScrollPos()
{
    m_scrollbar->setValue(m_wBaseAddress);
}

static const char * GetMemoryModeName(int mode)
{
    if (mode < 0 || mode > MEMMODE_LAST)
        return "UKWN";  // Unknown mode
    return MemoryView_ModeNames[mode];
}

void QMemoryView::updateWindowText()
{
    QString buffer = QString(tr("Memory - %1")).arg(GetMemoryModeName(m_Mode));
    parentWidget()->setWindowTitle(buffer);
}

void QMemoryView::updateData()
{
}

void QMemoryView::focusInEvent(QFocusEvent *)
{
    repaint();  // Need to draw focus rect
}
void QMemoryView::focusOutEvent(QFocusEvent *)
{
    repaint();  // Need to draw focus rect
}

void QMemoryView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction("Go to Address...", this, SLOT(gotoAddress()));
    menu.addSeparator();

    for (int mode = 0; mode <= MEMMODE_LAST; mode++)
    {
        const char * modeName = MemoryView_ModeNames[mode];
        QAction * action = menu.addAction(modeName, this, SLOT(changeMemoryMode()));
        action->setCheckable(true);
        action->setData(mode);
        if (m_Mode == mode)
            action->setChecked(true);
    }

    menu.addSeparator();
    menu.addAction("Words / Bytes", this, SLOT(changeWordByteMode()));

    menu.exec(event->globalPos());
}

void QMemoryView::changeMemoryMode()
{
    QAction * action = qobject_cast<QAction*>(sender());
    if (action == nullptr) return;
    int mode = action->data().toInt();
    if (mode < 0 || mode > MEMMODE_LAST) return;

    m_Mode = mode;
    Settings_SetDebugMemoryMode(m_Mode);

    repaint();
    updateWindowText();
}

void QMemoryView::changeWordByteMode()
{
    m_ByteMode = !m_ByteMode;
    Settings_SetDebugMemoryByte(m_ByteMode);

    repaint();
}

void QMemoryView::scrollBy(qint16 delta)
{
    if (delta == 0) return;

    m_wBaseAddress = (quint16)(m_wBaseAddress + delta);
    m_wBaseAddress = m_wBaseAddress & ((quint16)~15);
    Settings_SetDebugMemoryAddress(m_wBaseAddress);

    repaint();
    updateScrollPos();
}

void QMemoryView::gotoAddress()
{
    quint16 value = m_wBaseAddress;
    QInputOctalDialog dialog(this, "Go To Address", "Address (octal):", &value);
    if (dialog.exec() == QDialog::Rejected) return;

    // Scroll to the address
    m_wBaseAddress = value & ((quint16)~15);
    Settings_SetDebugMemoryAddress(m_wBaseAddress);

    repaint();
    updateScrollPos();
}

void QMemoryView::resizeEvent(QResizeEvent *)
{
    int cxScroll = this->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    m_scrollbar->setGeometry(this->width() - cxScroll, 0, cxScroll, this->height());
    m_scrollbar->setPageStep((this->height() / m_cyLine - 2) * 16);
}

void QMemoryView::scrollValueChanged()
{
    int value = m_scrollbar->value();
    m_wBaseAddress = (unsigned short)value & ((quint16)~15);
    Settings_SetDebugMemoryAddress(m_wBaseAddress);

    this->repaint();
}

void QMemoryView::paintEvent(QPaintEvent * /*event*/)
{
    if (g_pBoard == nullptr) return;

    QColor colorBackground = palette().color(QPalette::Base);
    QPainter painter(this);
    painter.fillRect(0, 0, this->width(), this->height(), colorBackground);

    QFont font = Common_GetMonospacedFont();
    painter.setFont(font);
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();

    QColor colorText = palette().color(QPalette::Text);
    QColor colorRed = Common_GetColorShifted(palette(), COLOR_RED);
    QColor colorBlue = Common_GetColorShifted(palette(), COLOR_BLUE);

    m_cyLineMemory = cyLine;

    char buffer[7];
    const char* ADDRESS_LINE = "  addr   0      2      4      6      10     12     14     16";
    painter.drawText(30, cyLine, ADDRESS_LINE);

    // Calculate m_nPageSize
    m_nPageSize = this->height() / cyLine - 1;

    quint16 address = m_wBaseAddress;
    int y = 2 * cyLine;
    for (;;)    // Draw lines
    {
        DrawOctalValue(painter, 30 + 1 * cxChar, y, address);

        int x = 30 + 9 * cxChar;
        ushort wchars[16];

        for (int j = 0; j < 8; j++)    // Draw words as octal value
        {
            // Get word from memory
            quint16 word = 0;
            int okValid = true;
            int addrtype = ADDRTYPE_NONE;
            bool okHalt = false;
            quint16 wChanged = 0;
            switch (m_Mode)
            {
            case MEMMODE_RAM0:
            case MEMMODE_RAM1:
            case MEMMODE_RAM2:
                word = g_pBoard->GetRAMWord(m_Mode, address);
                wChanged = Emulator_GetChangeRamStatus(m_Mode, address);
                break;
            case MEMMODE_ROM:  // ROM - only 32 Kbytes
                if (address < 0100000)
                    okValid = false;
                else
                {
                    addrtype = ADDRTYPE_ROM;
                    word = g_pBoard->GetROMWord(address - 0100000);
                }
                break;
            case MEMMODE_CPU:
                okHalt = g_pBoard->GetCPU()->IsHaltMode();
                word = g_pBoard->GetCPUMemoryController()->GetWordView(address, okHalt, false, &addrtype);
                okValid = (addrtype != ADDRTYPE_IO) && (addrtype != ADDRTYPE_DENY);
                wChanged = Emulator_GetChangeRamStatus(ADDRTYPE_RAM12, address);
                break;
            case MEMMODE_PPU:
                okHalt = g_pBoard->GetPPU()->IsHaltMode();
                word = g_pBoard->GetPPUMemoryController()->GetWordView(address, okHalt, false, &addrtype);
                okValid = (addrtype != ADDRTYPE_IO) && (addrtype != ADDRTYPE_DENY);
                if (address < 0100000)
                    wChanged = Emulator_GetChangeRamStatus(ADDRTYPE_RAM0, address);
                else
                    wChanged = 0;
                break;
            }

            if (okValid)
            {
                if (addrtype == ADDRTYPE_ROM)
                    painter.setPen(colorBlue);
                else
                    painter.setPen(wChanged != 0 ? colorRed : colorText);
                if (m_ByteMode)
                {
                    PrintOctalValue(buffer, (word & 0xff));
                    painter.drawText(x, y, buffer + 3);
                    PrintOctalValue(buffer, (word >> 8));
                    painter.drawText(x + 3 * cxChar + cxChar / 2, y, buffer + 3);
                }
                else
                    DrawOctalValue(painter, x, y, word);
            }

            // Prepare characters to draw at right
            quint8 ch1 = (quint8)(word & 0xff);
            ushort wch1 = Translate_KOI8R(ch1);
            if (ch1 < 32) wch1 = 0x00b7;
            wchars[j * 2] = wch1;
            quint8 ch2 = (quint8)((word >> 8) & 0xff);
            ushort wch2 = Translate_KOI8R(ch2);
            if (ch2 < 32) wch2 = 0x00b7;
            wchars[j * 2 + 1] = wch2;

            address += 2;
            x += 7 * cxChar;
        }
        painter.setPen(colorText);

        // Draw characters at right
        int xch = x + cxChar;
        QString wstr = QString::fromUtf16(wchars, 16);
        painter.drawText(xch, y, wstr);

        y += cyLine;
        if (y > this->height()) break;
    }  // Draw lines

    // Draw focus rect
    if (hasFocus())
    {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.state |= QStyle::State_KeyboardFocusChange;
        option.backgroundColor = QColor(Qt::gray);
        option.rect = QRect(30, cyLine + fontmetrics.descent(), 83 * cxChar, cyLine * m_nPageSize);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
    }
}

void QMemoryView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Space:
        if (event->isAutoRepeat()) return;
        event->accept();
        if (m_Mode == MEMMODE_LAST)
            m_Mode = 0;
        else
            m_Mode++;
        Settings_SetDebugMemoryMode(m_Mode);
        this->repaint();
        updateWindowText();
        break;

    case Qt::Key_G:
        event->accept();
        gotoAddress();
        break;
    case Qt::Key_B:
        event->accept();
        changeWordByteMode();
        break;

    case Qt::Key_Up:
        event->accept();
        scrollBy(-16);
        break;
    case Qt::Key_Down:
        event->accept();
        scrollBy(16);
        break;

    case Qt::Key_PageUp:
        event->accept();
        scrollBy(-m_nPageSize * 16);
        break;
    case Qt::Key_PageDown:
        event->accept();
        scrollBy(m_nPageSize * 16);
        break;
    }
}

void QMemoryView::wheelEvent(QWheelEvent * event)
{
    if (event->orientation() == Qt::Horizontal)
        return;
    event->accept();

    int steps = -event->delta() / 60;
    scrollBy(steps * 16);
}
