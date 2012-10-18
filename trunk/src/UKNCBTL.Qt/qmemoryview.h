#ifndef QMEMORYVIEW_H
#define QMEMORYVIEW_H

#include <QWidget>
#include <QScrollBar>

class QMemoryView : public QWidget
{
    Q_OBJECT
public:
    QMemoryView();
    ~QMemoryView();

    void updateData();
    void updateWindowText();

public slots:
    void changeMemoryMode();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *);
    void contextMenuEvent(QContextMenuEvent *);

protected slots:
    void scrollValueChanged();

private:
    int m_Mode;
    unsigned short m_wBaseAddress;
    int m_cyLineMemory;  // Line height in pixels
    int m_nPageSize;  // Page size in lines
    int m_cyLine;
    QScrollBar *m_scrollbar;
};


#endif // QMEMORYVIEW_H
