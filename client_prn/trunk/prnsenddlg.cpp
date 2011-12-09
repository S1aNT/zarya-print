#include "prnsenddlg.h"
#include "ui_prnsenddlg.h"

PrnSendDlg::PrnSendDlg(QWidget *parent)
    :QDialog(parent)
    ,m_ui(new Ui::PrnSendDlg)
    ,m_copy(0)
{
    m_ui->setupUi(this);
    connect(m_ui->clearButton,SIGNAL(clicked()),this,SIGNAL(clearDoc())
            );
    connect(m_ui->dirtyButton,SIGNAL(clicked()),this,SIGNAL(dirtyDoc())
            );
    connect(m_ui->clearButton,SIGNAL(clicked()),this,SLOT(disableButtons()));
    connect(m_ui->dirtyButton,SIGNAL(clicked()),this,SLOT(disableButtons()));
}

PrnSendDlg::~PrnSendDlg()
{
    delete m_ui;
}

void PrnSendDlg::showPrnState(const QString &info)
{
   m_ui->plainTextEdit->appendPlainText(info);
}

void PrnSendDlg::printNextCopy()
{
    m_copy++;
    m_ui->progressBar->setValue(m_copy);
}

void PrnSendDlg::enableButtons()
{
    m_ui->clearButton->setEnabled(true);
    m_ui->dirtyButton->setEnabled(true);
}
void PrnSendDlg::disableButtons()
{
    m_ui->clearButton->setEnabled(false);
    m_ui->dirtyButton->setEnabled(false);
}

void PrnSendDlg::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
