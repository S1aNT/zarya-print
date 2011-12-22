#include "prnsenddlg.h"
#include "ui_prnsenddlg.h"
#include "mytypes.h"
using namespace VPrn;

PrnSendDlg::PrnSendDlg(QWidget *parent)
        :QDialog(parent)
        ,m_ui(new Ui::PrnSendDlg)
        ,m_copy(1)
{
    m_ui->setupUi(this);
    m_ui->progressBar->setRange(1,5);

    signalMapper = new QSignalMapper(this);

    connect(m_ui->clearButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(m_ui->dirtyButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

    signalMapper->setMapping(m_ui->clearButton, 0);
    signalMapper->setMapping(m_ui->dirtyButton, 1);

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(do_clicked(int)));    
}

PrnSendDlg::~PrnSendDlg()
{
    delete m_ui;
}

void PrnSendDlg::checkNextCopy()
{
    if (m_copy <VPrn::FirstPageN5){
        m_ui->progressBar->setValue(m_copy);
        emit getNextCopy(m_copy);
    }
}

void PrnSendDlg::showPrnState(const QString &info)
{
    m_ui->plainTextEdit->appendPlainText(info);
}

void PrnSendDlg::do_needMarkDoc()
{
    this->setButtons(true);
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

void PrnSendDlg::do_clicked(int btn)
{
    switch (btn){
    case 0:
        emit clearDoc();
        break;
    case 1:
        emit dirtyDoc();
        break;
    default:
        break;
    }
    m_copy++;
    this->checkNextCopy();

    this->setButtons(false);
}

void PrnSendDlg::setButtons(bool state)
{
    m_ui->clearButton->setEnabled(state);
    m_ui->dirtyButton->setEnabled(state);
}


