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
    CProcessor* getCurrentProc();
    bool isCpuOrPpu();
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
    QHBoxLayout* m_hlayout;
    QDebugCtrl* m_procCtrl;
    QDebugCtrl* m_stackCtrl;
    QDebugCtrl* m_portsCtrl;
    QDebugCtrl* m_breaksCtrl;
    QDebugCtrl* m_memmapCtrl;
    bool m_okDebugProcessor;        // TRUE - CPU, FALSE - PPU
};

class QDebugCtrl : public QWidget
{
    Q_OBJECT
public:
    QDebugCtrl(QDebugView *debugView);

    virtual void updateData() { }

protected:
    QDebugView *m_pDebugView;

protected:
    CProcessor* getProc() { return m_pDebugView->getCurrentProc(); }
    bool isCpuOrPpu() { return m_pDebugView->isCpuOrPpu(); }
};

class QDebugProcessorCtrl : public QDebugCtrl
{
    Q_OBJECT
public:
    QDebugProcessorCtrl(QDebugView *debugView);

    virtual void updateData();

protected:
    void paintEvent(QPaintEvent *event);

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
