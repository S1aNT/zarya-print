#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPixmap>
#include <QtGui/QWidget>

#include "mytypes.h"
#include "intropage.h"

using namespace VPrn;

IntroPage::IntroPage(QWidget *parent)
    : QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
    this->setWindowTitle(QObject::trUtf8("Инициализация подсистем Защищенного принтера."));

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    //Формируем левую часть блока
    QLabel *label = new QLabel();
    label->setPixmap(QPixmap(":/printer_logo.png"));

    horizontalLayout->addWidget(label);
    horizontalLayout->addStretch(0);
    horizontalLayout->addLayout(verticalLayout);
    //Формируем правую часть блока
    QLabel *topLabel = new QLabel(QObject::trUtf8("Этот мастер поможет Вам сделать <i>твердую копию</i> "
                                                  "документа используя <b>Защищенный виртуальный принтер</b>&trade")
                                  );
    topLabel->setWordWrap(true);

    inputFile_step          = new QCheckBox(this);
    inputFile_step->setText(QObject::trUtf8("Задан файл для печати"));
    inputFile_step->setEnabled(false);

    getAuthData_step         = new QCheckBox(this);
    getAuthData_step->setText(QObject::trUtf8("Получен контекст безопастности пользователя"));
    getAuthData_step->setEnabled(false);

    loadPlugin_step          = new QCheckBox(this);
    loadPlugin_step->setText(QObject::trUtf8("Необходимые плагины загружены"));
    loadPlugin_step->setEnabled(false);

    connect2RemoteDemon_step = new QCheckBox(this);
    connect2RemoteDemon_step->setText(QObject::trUtf8("Установлена связь с Упр. сервером"));
    connect2RemoteDemon_step->setEnabled(false);

    authPassed_step      = new QCheckBox(this);
    authPassed_step->setText(QObject::trUtf8("Пользователь авторизирован на Упр. сервере"));
    authPassed_step->setEnabled(false);


    reciveSecLabel_step      = new QCheckBox(this);
    reciveSecLabel_step->setText(QObject::trUtf8("Получен список уровней секретности связанных с мандатом"));
    reciveSecLabel_step->setEnabled(false);

    recivePrintersList_step  = new QCheckBox(this);
    recivePrintersList_step->setText(QObject::trUtf8("Получен список принтеров, доступных пользователю"));
    recivePrintersList_step->setEnabled(false);


    verticalLayout->addStretch(0);
    verticalLayout->addWidget(topLabel);
    verticalLayout->addSpacing(3);

    verticalLayout->addWidget(inputFile_step);
    verticalLayout->addWidget(getAuthData_step);
    verticalLayout->addWidget(loadPlugin_step);
    verticalLayout->addWidget(connect2RemoteDemon_step);
    verticalLayout->addWidget(authPassed_step);
    verticalLayout->addWidget(reciveSecLabel_step);
    verticalLayout->addWidget(recivePrintersList_step);
    verticalLayout->addStretch(0);

    setLayout(horizontalLayout);
}

void IntroPage::setStep(VPrn::InitSteps step)
{
    switch (step){
    case VPrn::step_inputFile:
        inputFile_step->setChecked(true);
        break;
    case VPrn::step_defineAuthData:
        getAuthData_step->setChecked(true);
        break;
    case VPrn::step_loadPlugin:
        loadPlugin_step->setChecked(true);
        break;
    case VPrn::step_connect2RemoteDemon:
        connect2RemoteDemon_step->setChecked(true);
        break;
    case VPrn::step_authPassed:
        authPassed_step->setChecked(true);
        break;
    case VPrn::step_reciveSecLabel:
        reciveSecLabel_step->setChecked(true);
        break;
    case VPrn::step_recivePrintersList:
        recivePrintersList_step->setChecked(true);
        break;
    }
    this->checkSteps();
}

void IntroPage::checkSteps()
{
    bool ok = inputFile_step->isChecked() &&
            getAuthData_step->isChecked() &&
            loadPlugin_step->isChecked() &&
            connect2RemoteDemon_step->isChecked() &&
            authPassed_step->isChecked() &&
            reciveSecLabel_step->isChecked() &&
            recivePrintersList_step->isChecked();
    if (ok){
        emit enableNext();
    }
}
