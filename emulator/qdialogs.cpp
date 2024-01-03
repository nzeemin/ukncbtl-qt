#include "stdafx.h"
#include "qdialogs.h"
#include <QMessageBox>


//////////////////////////////////////////////////////////////////////


QInputOctalDialog::QInputOctalDialog(QWidget *parent, const QString & title, quint16 * value)
    : QDialog(parent, nullptr)
{
    m_result = value;

    QFont font = Common_GetMonospacedFont();
    m_editOctal.setFont(font);
    m_editHex.setFont(font);

    setWindowTitle(title);
    resize(260, 120);
    m_labelOctal.setText(tr("Octal"));
    m_labelHex.setText(tr("Hex"));
    m_buttons.setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    m_layout.addWidget(&m_labelOctal, 0, 0);
    m_layout.addWidget(&m_labelHex, 0, 1);
    m_layout.addWidget(&m_editOctal, 1, 0);
    m_layout.addWidget(&m_editHex, 1, 1);
    m_layout.addWidget(&m_spacer, 2, 0);
    m_layout.addWidget(&m_buttons, 3, 0, 1, -1);
    setLayout(&m_layout);

    QObject::connect(&m_editOctal, SIGNAL(textEdited(QString)), this, SLOT(octalEdited(QString)));
    QObject::connect(&m_editHex, SIGNAL(textEdited(QString)), this, SLOT(hexEdited(QString)));
    QObject::connect(&m_buttons, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(&m_buttons, SIGNAL(accepted()), this, SLOT(accept()));

    char buffer[8];
    PrintOctalValue(buffer, *value);
    m_editOctal.setText(buffer);
    PrintHexValue(buffer, *value);
    m_editHex.setText(buffer);
    m_editOctal.selectAll();
}

void QInputOctalDialog::octalEdited(const QString &text)
{
    quint16 value;
    if (! ParseOctalValue(text.toLatin1().data(), &value))
    {
        m_editHex.setText(nullptr);
    }
    else
    {
        char buffer[8];
        PrintHexValue(buffer, value);
        m_editHex.setText(buffer);
    }
}

void QInputOctalDialog::hexEdited(const QString &text)
{
    quint16 value;
    if (! ParseHexValue(text.toLatin1().data(), &value))
    {
        m_editOctal.setText(nullptr);
    }
    else
    {
        char buffer[8];
        PrintOctalValue(buffer, value);
        m_editOctal.setText(buffer);
    }
}

void QInputOctalDialog::accept()
{
    QString text = m_editOctal.text();
    quint16 value;
    if (text.isEmpty() || !ParseOctalValue(text.toLatin1().data(), &value))
    {
        QMessageBox::warning(this, nullptr, tr("Please enter correct octal value."));
        return;
    }

    *m_result = value;
    QDialog::accept();
}


//////////////////////////////////////////////////////////////////////


QAboutDialog::QAboutDialog(QWidget * parent)
    : QDialog(parent)
{
    setWindowTitle(tr("About"));
    setMinimumSize(480, 340);

    QLabel * logoLabel = new QLabel();
    logoLabel->setPixmap(QPixmap(":/images/ukncbtl.png"));

    QLabel * versionLabel = new QLabel(tr(
            "UKNCBTL Qt Version 1.0\nCopyright (C) 2007-2024\n"));

    QLabel * authorsLabel = new QLabel(tr(
            "Authors:\r\nNikita Zimin\nFelix Lazarev\nAlexey Kisly\n\n"
            "Special thanks to:\nArseny Gordin"));

    QLabel * linkLabel = new QLabel(
        "<a href=\"https://github.com/nzeemin/ukncbtl-qt\">https://github.com/nzeemin/ukncbtl-qt</a>");
    linkLabel->setOpenExternalLinks(true);

    QLabel * disclamerLabel = new QLabel(tr(
            "This program is provided by authors and contributors AS IS, with absolutely no warranty of any kind.  Please use it on your own risk."));
    disclamerLabel->setWordWrap(true);

    QLabel * bottomLabel = new QLabel(
        tr("Build date:\t%1 %2\nQt version:\t%3").arg(__DATE__).arg(__TIME__).arg(QT_VERSION_STR));

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QGridLayout * layout = new QGridLayout(this);
    layout->addWidget(logoLabel, 0, 0, 5, 1, Qt::AlignTop);
    layout->addWidget(versionLabel, 0, 1);
    layout->addWidget(authorsLabel, 1, 1);
    layout->addWidget(linkLabel, 2, 1);
    layout->addWidget(disclamerLabel, 3, 1);
    layout->addWidget(bottomLabel, 4, 1);
    layout->addWidget(buttonBox, 5, 1);
    layout->setColumnMinimumWidth(0, 136);
    layout->setColumnMinimumWidth(1, 250);

    setLayout(layout);
}


//////////////////////////////////////////////////////////////////////
