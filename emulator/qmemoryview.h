#ifndef QMEMORYVIEW_H
#define QMEMORYVIEW_H

#include <QWidget>

class QScrollBar;
class QToolBar;

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
    void changeWordByteMode();
    void changeNumeralMode();
    void gotoAddress();
    void scrollBy(qint16 delta);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);
    void wheelEvent(QWheelEvent *);

    void updateScrollPos();
    quint16 getWordFromMemory(quint16 address, bool& okValid, int& addrtype, quint16& wChanged);

protected slots:
    void scrollValueChanged();

private:
    int m_Mode;
    bool m_ByteMode;  // false - word mode, true - byte mode
    quint16 m_NumeralMode = 0;
    quint16 m_wBaseAddress;
    int m_cyLineMemory;  // Line height in pixels
    int m_nPageSize;  // Page size in lines
    int m_cyLine;
    int m_PostionIncrement = 100;  // Increment by X to the next word
    QScrollBar *m_scrollbar;
    QToolBar* m_toolbar;
};


#endif // QMEMORYVIEW_H
