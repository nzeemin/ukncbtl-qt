#ifndef QDIALOGS_H
#define QDIALOGS_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

class QWidget;


//////////////////////////////////////////////////////////////////////


class QInputOctalDialog : public QDialog
{
    Q_OBJECT

public:
    QInputOctalDialog(QWidget * parent, const QString & title, quint16 * value);

public slots:
    void octalEdited(const QString &text);
    void hexEdited(const QString &text);
    virtual void accept();

private:
    quint16 * m_result;
    QGridLayout m_layout;
    QLabel m_labelOctal;
    QLabel m_labelHex;
    QLineEdit m_editOctal;
    QLineEdit m_editHex;
    QWidget m_spacer;
    QDialogButtonBox m_buttons;
};


//////////////////////////////////////////////////////////////////////


class QAboutDialog : public QDialog
{
    Q_OBJECT

public:
    QAboutDialog(QWidget * parent);
};


//////////////////////////////////////////////////////////////////////
#endif // QDIALOGS_H
