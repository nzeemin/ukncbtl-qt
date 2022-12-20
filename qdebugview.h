#ifndef QDEBUGVIEW_H
#define QDEBUGVIEW_H

#include <QWidget>
#include <QHBoxLayout>

class CMotherboard;
class CProcessor;
class CMemoryController;
class QToolBar;
class QDebugCtrl;

class QDebugView : public QWidget
{
    Q_OBJECT
public:
    QDebugView(QWidget *mainWindow);

    void setCurrentProc(bool okProc);
    CProcessor* getCurrentProc() const;
    bool isCpuOrPpu() const;
    void updateData();
    void updateWindowText();

public slots:
    void switchCpuPpu();

protected:
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);

private:
    QToolBar* m_toolbar;
    QDebugCtrl* m_procCtrl;
    QDebugCtrl* m_stackCtrl;
    QDebugCtrl* m_portsCtrl;
    QDebugCtrl* m_breaksCtrl;
    QDebugCtrl* m_memmapCtrl;
    bool m_okDebugProcessor;        // TRUE - CPU, FALSE - PPU
};

struct DebugCtrlHitTest
{
    bool        isValid;
    int         line;       // Line number
    uint16_t    address;
    uint16_t    value;
};

class QDebugCtrl : public QWidget
{
    Q_OBJECT
public:
    QDebugCtrl(QDebugView *debugView);

    virtual void updateData() { }
    virtual DebugCtrlHitTest hitTest(int x, int y);

public slots:
    void copyAddressOctal();
    void copyValueOctal();
    void copyValueHex();
    void copyValueBinary();

protected:
    QDebugView *m_pDebugView;
    DebugCtrlHitTest m_lastHitTest;

protected:
    CProcessor* getProc() const { return m_pDebugView->getCurrentProc(); }
    bool isCpuOrPpu() const { return m_pDebugView->isCpuOrPpu(); }
};

class QDebugProcessorCtrl : public QDebugCtrl
{
    Q_OBJECT
public:
    QDebugProcessorCtrl(QDebugView *debugView);

    virtual void updateData();

protected:
    void paintEvent(QPaintEvent *event);
    virtual DebugCtrlHitTest hitTest(int x, int y);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    unsigned short m_wDebugCpuR[9];  // Old register values - R0..R7, PSW
    unsigned short m_wDebugPpuR[9];  // Old register values - R0..R7, PSW
    bool m_okDebugCpuRChanged[9];   // Register change flags
    bool m_okDebugPpuRChanged[9];   // Register change flags
};

class QDebugStackCtrl : public QDebugCtrl
{
    Q_OBJECT
public:
    QDebugStackCtrl(QDebugView *debugView);

    virtual void updateData();
    virtual DebugCtrlHitTest hitTest(int x, int y);
    void contextMenuEvent(QContextMenuEvent *event);

protected:
    void paintEvent(QPaintEvent *event);

protected:
    quint16 m_wDebugCpuR6Old;  // SP value on previous step
    quint16 m_wDebugPpuR6Old;  // SP value on previous step
};

class QDebugPortsCtrl : public QDebugCtrl
{
    Q_OBJECT
public:
    QDebugPortsCtrl(QDebugView *debugView);

protected:
    void paintEvent(QPaintEvent *event);
};

class QDebugBreakpointsCtrl : public QDebugCtrl
{
    Q_OBJECT
public:
    QDebugBreakpointsCtrl(QDebugView *debugView);

protected:
    void paintEvent(QPaintEvent *event);
};

class QDebugMemoryMapCtrl : public QDebugCtrl
{
    Q_OBJECT
public:
    QDebugMemoryMapCtrl(QDebugView *debugView);

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // QDEBUGVIEW_H
