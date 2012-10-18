#include "stdafx.h"
#include <QtGui>
#include <QPainter>
#include <QScrollBar>
#include "qmemoryview.h"
#include "Emulator.h"
#include "emubase/Emubase.h"


enum MemoryViewMode {
    MEMMODE_RAM0 = 0,  // RAM plane 0
    MEMMODE_RAM1 = 1,  // RAM plane 1
    MEMMODE_RAM2 = 2,  // RAM plane 2
    MEMMODE_ROM  = 3,  // ROM
    MEMMODE_CPU  = 4,  // CPU memory
    MEMMODE_PPU  = 5,  // PPU memory
    MEMMODE_LAST = 5   // Last mode number
};

static const char * MemoryView_ModeNames[] = {
    "RAM0", "RAM1", "RAM2", "ROM", "CPU", "PPU"
};


QMemoryView::QMemoryView()
{
    m_Mode = MEMMODE_ROM;
    m_wBaseAddress = 0;
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

    setFocusPolicy(Qt::StrongFocus);
}

QMemoryView::~QMemoryView()
{
    delete m_scrollbar;
}

const char * GetMemoryModeName(int mode)
{
    if (mode < 0 || mode > MEMMODE_LAST)
        return _T("UKWN");  // Unknown mode
    return MemoryView_ModeNames[mode];
}

void QMemoryView::updateWindowText()
{
    QString buffer = QString(_T("Memory - %1")).arg(GetMemoryModeName(m_Mode));
    parentWidget()->setWindowTitle(buffer);
}

void QMemoryView::updateData()
{
}

void QMemoryView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    //menu.addAction("Go to Address...");
    //menu.addSeparator();

    for (int mode = 0; mode <= MEMMODE_LAST; mode++)
    {
        const char * modeName = MemoryView_ModeNames[mode];
        QAction * action = menu.addAction(modeName, this, SLOT(changeMemoryMode()));
        action->setCheckable(true);
        action->setData(mode);
        if (m_Mode == mode)
            action->setChecked(true);
    }

    menu.exec(event->globalPos());
}

void QMemoryView::changeMemoryMode()
{
    QAction * action = qobject_cast<QAction*>(sender());
    if (action == 0) return;
    int mode = action->data().toInt();
    if (mode < 0 || mode > MEMMODE_LAST) return;

    m_Mode = mode;
    repaint();
    updateWindowText();
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
    m_wBaseAddress = (unsigned short)value & ~15;
    this->repaint();
}

void QMemoryView::paintEvent(QPaintEvent * /*event*/)
{
    if (g_pBoard == NULL) return;

    QPainter painter(this);
    painter.fillRect(0,0, this->width(), this->height(), Qt::white);

    QFont font = Common_GetMonospacedFont();
    painter.setFont(font);
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();

    QColor colorText = painter.pen().color();

    m_cyLineMemory = cyLine;

    const TCHAR* ADDRESS_LINE = _T(" address  0      2      4      6      10     12     14     16");
    painter.drawText(0, cyLine, ADDRESS_LINE);

    // Calculate m_nPageSize
    m_nPageSize = this->height() / cyLine - 1;

    WORD address = m_wBaseAddress;
    int y = 2 * cyLine;
    for (;;) {  // Draw lines
        DrawOctalValue(painter, 2 * cxChar, y, address);

        int x = 10 * cxChar;
        ushort wchars[16];

        for (int j = 0; j < 8; j++) {  // Draw words as octal value
            // Get word from memory
            WORD word = 0;
            BOOL okValid = TRUE;
            BOOL okHalt = FALSE;
            WORD wChanged = 0;
            switch (m_Mode) {
                case MEMMODE_RAM0:
                case MEMMODE_RAM1:
                case MEMMODE_RAM2:
                    word = g_pBoard->GetRAMWord(m_Mode, address);
                    wChanged = Emulator_GetChangeRamStatus(m_Mode, address);
                    break;
                case MEMMODE_ROM:  // ROM - only 32 Kbytes
                    if (address < 0100000)
                        okValid = FALSE;
                    else
                        word = g_pBoard->GetROMWord(address - 0100000);
                    break;
                case MEMMODE_CPU:
                    okHalt = g_pBoard->GetCPU()->IsHaltMode();
                    word = g_pBoard->GetCPUMemoryController()->GetWordView(address, okHalt, FALSE, &okValid);
                    wChanged = Emulator_GetChangeRamStatus(ADDRTYPE_RAM12, address);
                    break;
                case MEMMODE_PPU:
                    okHalt = g_pBoard->GetPPU()->IsHaltMode();
                    word = g_pBoard->GetPPUMemoryController()->GetWordView(address, okHalt, FALSE, &okValid);
                    if (address < 0100000)
                        wChanged = Emulator_GetChangeRamStatus(ADDRTYPE_RAM0, address);
                    else
                        wChanged = 0;
                    break;
                    break;
            }

            if (okValid)
            {
                painter.setPen(wChanged != 0 ? Qt::red : colorText);
                DrawOctalValue(painter, x, y, word);
            }

            // Prepare characters to draw at right
            BYTE ch1 = LOBYTE(word);
            ushort wch1 = Translate_KOI8R(ch1);
            if (ch1 < 32) wch1 = 0x00b7;
            wchars[j * 2] = wch1;
            BYTE ch2 = HIBYTE(word);
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
    }
}

void QMemoryView::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_Space)
    {
        event->accept();
        if (m_Mode == MEMMODE_LAST)
            m_Mode = 0;
        else
            m_Mode++;
        this->repaint();
        updateWindowText();
    }
}
