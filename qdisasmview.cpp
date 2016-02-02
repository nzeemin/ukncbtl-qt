#include "stdafx.h"
#include <QtGui>
#include <QFileDialog>
#include <QMenu>
#include <QStyleOptionFocusRect>
#include "main.h"
#include "qdisasmview.h"
#include "Emulator.h"
#include "emubase/Emubase.h"

#define COLOR_SUBTITLE  qRgb(0,128,0)

QDisasmView::QDisasmView()
{
    m_okDisasmProcessor = FALSE;
    m_wDisasmBaseAddr = 0;
    m_wDisasmNextBaseAddr = 0;

    QFont font = Common_GetMonospacedFont();
    QFontMetrics fontmetrics(font);
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();
    this->setMinimumSize(cxChar * 55, cyLine * 10 + cyLine / 2);

    setFocusPolicy(Qt::ClickFocus);
}

void QDisasmView::setCurrentProc(bool okProc)
{
    m_okDisasmProcessor = okProc;
    this->updateData();
}

void QDisasmView::updateData()
{
    CProcessor* pDisasmPU = (m_okDisasmProcessor) ? g_pBoard->GetCPU() : g_pBoard->GetPPU();
    ASSERT(pDisasmPU != NULL);
    m_wDisasmBaseAddr = pDisasmPU->GetPC();
}

void QDisasmView::focusInEvent(QFocusEvent *)
{
    repaint();  // Need to draw focus rect
}
void QDisasmView::focusOutEvent(QFocusEvent *)
{
    repaint();  // Need to draw focus rect
}

void QDisasmView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(m_okDisasmProcessor ? "Switch to PPU" : "Switch to CPU", this, SLOT(switchCpuPpu()));
    menu.addAction(m_SubtitleItems.isEmpty() ? "Show Subtitles..." : "Hide Subtitles", this, SLOT(showHideSubtitles()));
    menu.exec(event->globalPos());
}

void QDisasmView::switchCpuPpu()
{
    Global_SetCurrentProc(! m_okDisasmProcessor);
    Global_UpdateAllViews();
}

void QDisasmView::showHideSubtitles()
{
    if (m_SubtitleItems.size() > 0)
    {
        m_SubtitleItems.clear();
    }
    else
    {
        QFileDialog dlg;
        dlg.setNameFilter(_T("UKNCBTL subtitles (*.lst)"));
        if (dlg.exec() == QDialog::Rejected)
            return;
        QString fileName = dlg.selectedFiles().at(0);

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            AlertWarning(_T("Failed to open the file."));
            return;
        }

        QTextStream stream(&file);
        parseSubtitles(stream);
    }

    //TODO: updateTitle();
    repaint();
}

void QDisasmView::addSubtitle(quint16 addr, DisasmSubtitleType type, const QString &comment)
{
    DisasmSubtitleItem item;
    item.address = addr;
    item.type = type;
    item.comment = comment;
    m_SubtitleItems.append(item);
}

void QDisasmView::parseSubtitles(QTextStream &stream)
{
    QString blockComment;

    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        int lineLength = line.length();

        if (lineLength == 0) continue;  // Skip empty lines

        QChar firstChar = line.at(0);
        if (firstChar.isDigit())  // Цифра -- считаем что это адрес
        {
            // Parse address
            int addrlen = 1;
            while (addrlen < lineLength && line.at(addrlen).isDigit()) addrlen++;
            quint16 address;
            if (!ParseOctalValue(line.left(addrlen), &address))
                continue;

            if (!blockComment.isEmpty())  // На предыдущей строке был комментарий к блоку
            {
                addSubtitle(address, SUBTYPE_BLOCKCOMMENT, blockComment);
                blockComment.clear();
            }

            // Skip separators
            int index = addrlen;
            while (index < lineLength)
            {
                QChar ch = line.at(index);
                if (ch == ' ' || ch == '\t' || ch == '$' || ch == ':')
                    index++;
                else
                    break;
            }
            if (index == lineLength) continue;

            // Search for comment start
            while (index < lineLength)
            {
                QChar ch = line.at(index);
                if (ch == ';')
                    break;
                index++;
            }
            if (index == lineLength) continue;

            QString comment = line.mid(index).trimmed();
            if (comment.length() > 1)
            {
                addSubtitle(address, SUBTYPE_COMMENT, comment);
            }
        }
        else if (firstChar == ';')
        {
            blockComment = line.trimmed();
            //TODO: Собирать многострочные комментарии над блоком
        }
    }
}

void QDisasmView::paintEvent(QPaintEvent * /*event*/)
{
    if (g_pBoard == NULL) return;

    QPainter painter(this);
    painter.fillRect(0,0, this->width(), this->height(), Qt::white);

    QFont font = Common_GetMonospacedFont();
    painter.setFont(font);
    QFontMetrics fontmetrics(font);
    int cyLine = fontmetrics.height();

    CProcessor* pDisasmPU = (m_okDisasmProcessor) ? g_pBoard->GetCPU() : g_pBoard->GetPPU();
    ASSERT(pDisasmPU != NULL);

    // Draw disasseble for the current processor
    quint16 prevPC = (m_okDisasmProcessor) ? g_wEmulatorPrevCpuPC : g_wEmulatorPrevPpuPC;
    int yFocus = DrawDisassemble(painter, pDisasmPU, m_wDisasmBaseAddr, prevPC);

    // Draw focus rect
    if (hasFocus())
    {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.state |= QStyle::State_KeyboardFocusChange;
        option.backgroundColor = QColor(Qt::gray);
        option.rect = QRect(0, yFocus - cyLine + 1, 1000, cyLine);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
    }
}

const DisasmSubtitleItem * QDisasmView::findSubtitle(quint16 address, quint16 typemask)
{
    if (m_SubtitleItems.isEmpty())
        return 0;

    const DisasmSubtitleItem * item = m_SubtitleItems.constData();
    while (item->type != 0)
    {
        if (item->address > address)
            return 0;
        if (item->address == address &&
            (item->type &typemask) != 0)
            return item;
        ++item;
    }

    return 0;
}

int QDisasmView::DrawDisassemble(QPainter &painter, CProcessor *pProc, unsigned short base, unsigned short previous)
{
    int result = -1;

    QFontMetrics fontmetrics(painter.font());
    int cxChar = fontmetrics.averageCharWidth();
    int cyLine = fontmetrics.height();
    QColor colorText = painter.pen().color();

    CMemoryController* pMemCtl = pProc->GetMemoryController();
    quint16 proccurrent = pProc->GetPC();
    quint16 current = base;

    // Читаем из памяти процессора в буфер
    const int nWindowSize = 30; //this->height() / cyLine;
    quint16 memory[nWindowSize + 2];
    for (int idx = 0; idx < nWindowSize; idx++) {
        bool valid;
        memory[idx] = pMemCtl->GetWordView(
                current + idx * 2 - 10, pProc->IsHaltMode(), TRUE, &valid);
    }

    quint16 address = current - 10;
    quint16 disasmfrom = current;
    if ((previous >= address) && previous < current)
        disasmfrom = previous;

    int length = 0;
    quint16 wNextBaseAddr = 0;
    int y = cyLine;
    for (int index = 0; index < nWindowSize; index++)  // Рисуем строки
    {
        if (!m_SubtitleItems.isEmpty())  // Subtitles - комментарий к блоку
        {
            const DisasmSubtitleItem * pSubItem = findSubtitle(address, SUBTYPE_BLOCKCOMMENT);
            if (pSubItem != NULL && !pSubItem->comment.isEmpty())
            {
                painter.setPen(QColor(COLOR_SUBTITLE));
                painter.drawText(21 * cxChar, y, pSubItem->comment);
                painter.setPen(colorText);

                y += cyLine;
            }
        }

        DrawOctalValue(painter, 5 * cxChar, y, address);  // Address
        // Value at the address
        quint16 value = memory[index];
        painter.setPen(Qt::gray);
        DrawOctalValue(painter, 13 * cxChar, y, value);
        painter.setPen(colorText);

        // Current position
        if (address == current)
        {
            painter.drawText(1 * cxChar, y, _T("  >"));
            result = y;  // Remember line for the focus rect
        }
        if (address == proccurrent)
        {
            bool okPCchanged = proccurrent != previous;
            if (okPCchanged) painter.setPen(Qt::red);
            painter.drawText(1 * cxChar, y, _T("PC"));
            painter.setPen(colorText);
            painter.drawText(3 * cxChar, y, _T(">>"));
        }
        else if (address == previous)
        {
            painter.setPen(Qt::blue);
            painter.drawText(1 * cxChar, y, _T("  >"));
        }

        bool okData = FALSE;
        if (!m_SubtitleItems.isEmpty())  // Show subtitle
        {
            const DisasmSubtitleItem* pSubItem = findSubtitle(address, SUBTYPE_COMMENT | SUBTYPE_DATA);
            if (pSubItem != NULL && (pSubItem->type & SUBTYPE_DATA) != 0)
                okData = TRUE;
            if (pSubItem != NULL && (pSubItem->type & SUBTYPE_COMMENT) != 0 && !pSubItem->comment.isEmpty())
            {
                painter.setPen(QColor(COLOR_SUBTITLE));
                painter.drawText(52 * cxChar, y, pSubItem->comment);
                painter.setPen(colorText);

                // Строку с субтитром мы можем использовать как опорную для дизассемблера
                if (disasmfrom > address)
                    disasmfrom = address;
            }
        }

        if (address >= disasmfrom && length == 0)
        {
            TCHAR strInstr[8];
            TCHAR strArg[32];
            if (okData)  // По этому адресу лежат данные -- нет смысла дизассемблировать
            {
                _tcscpy(strInstr, _T("data"));
                PrintOctalValue(strArg, *(memory + index));
                length = 1;
            }
            else
            {
                length = DisassembleInstruction(memory + index, address, strInstr, strArg);
            }
            if (index + length <= nWindowSize)
            {
                painter.drawText(21 * cxChar, y, strInstr);
                painter.drawText(29 * cxChar, y, strArg);
            }
            painter.setPen(colorText);
            if (wNextBaseAddr == 0)
                wNextBaseAddr = address + length * 2;
        }
        if (length > 0) length--;

        address += 2;
        y += cyLine;
    }

    m_wDisasmNextBaseAddr = wNextBaseAddr;

    return result;
}
