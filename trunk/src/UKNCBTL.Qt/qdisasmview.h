#ifndef QDISASMVIEW_H
#define QDISASMVIEW_H

#include <QWidget>

class QPainter;
class CProcessor;

class QDisasmView : public QWidget
{
    Q_OBJECT
public:
    QDisasmView();

    void setCurrentProc(bool okProc);
    void updateData();

public slots:
    void switchCpuPpu();

protected:
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    bool m_okDisasmProcessor;  // TRUE - CPU, FALSE - PPU
    unsigned short m_wDisasmBaseAddr;
    unsigned short m_wDisasmNextBaseAddr;

    void DrawDisassemble(QPainter& painter, CProcessor* pProc, unsigned short base, unsigned short previous);
};

#endif // QDISASMVIEW_H
