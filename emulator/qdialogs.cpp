#include "stdafx.h"
#include "qdialogs.h"
#include <QMessageBox>


//////////////////////////////////////////////////////////////////////


QInputOctalDialog::QInputOctalDialog(QWidget *parent, const QString & title, const QString & prompt, quint16 * value)
    : QDialog(parent, nullptr)
{
    m_result = value;

    char buffer[8];
    PrintOctalValue(buffer, *value);

    setWindowTitle(title);
    resize(340, 120);
    m_label.setText(prompt);
    m_layout.addWidget(&m_label);
    m_edit.setText(buffer);
    m_edit.selectAll();
    m_layout.addWidget(&m_edit);
    m_layout.addWidget(&m_spacer);
    m_buttons.setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(&m_buttons, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(&m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
    m_layout.addWidget(&m_buttons);
    setLayout(&m_layout);
}

void QInputOctalDialog::accept()
{
    quint16 value;
    if (! ParseOctalValue(m_edit.text().toLatin1().data(), &value))
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
