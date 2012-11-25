#ifndef QDISASMVIEW_H
#define QDISASMVIEW_H

#include <QVector>
#include <QTextStream>
#include <QWidget>

class QPainter;
class CProcessor;


enum DisasmSubtitleType
{
    SUBTYPE_NONE = 0,
    SUBTYPE_COMMENT = 1,
    SUBTYPE_BLOCKCOMMENT = 2,
    SUBTYPE_DATA = 4
};

struct DisasmSubtitleItem
{
    quint16 address;
    DisasmSubtitleType type;
    QString comment;
};

class QDisasmView : public QWidget
{
    Q_OBJECT
public:
    QDisasmView();

    void setCurrentProc(bool okProc);
    void updateData();

public slots:
    void switchCpuPpu();
    void showHideSubtitles();

protected:
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);

    void parseSubtitles(QTextStream& stream);
    void addSubtitle(quint16 addr, DisasmSubtitleType type, const QString& comment);

private:
    bool m_okDisasmProcessor;  // TRUE - CPU, FALSE - PPU
    unsigned short m_wDisasmBaseAddr;
    unsigned short m_wDisasmNextBaseAddr;
    QVector<DisasmSubtitleItem> m_SubtitleItems;

    int DrawDisassemble(QPainter& painter, CProcessor* pProc, unsigned short base, unsigned short previous);
    const DisasmSubtitleItem * findSubtitle(quint16 address, quint16 typemask);
};

#endif // QDISASMVIEW_H
