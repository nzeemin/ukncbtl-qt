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
    QInputOctalDialog(QWidget * parent, const QString & title, const QString & prompt, quint16 * value);

public slots:
    virtual void accept();

private:
    quint16 * m_result;
    QVBoxLayout m_layout;
    QLabel m_label;
    QLineEdit m_edit;
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
