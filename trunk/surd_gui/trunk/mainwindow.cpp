#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QSettings>

#include <QtGui/QCheckBox>
#include <QtGui/QApplication>

#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QGroupBox>
#include <QFont>
#include <QRegExp>
#include <QRegExpValidator>

#include <QtGui/QErrorMessage>
#include <QDataWidgetMapper>

#include "mainwindow.h"


using namespace VPrn;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , m_engine(0)
    , m_netClient(0)
    , serverName(QString())
    , serverPort(-1)

{

    this->setWindowTitle(QObject::trUtf8("Программа генерации запроса для ТД,СД,ОД"));
    //Основной потребитель ошибок
    myEMsgBox = new QErrorMessage(this);
    // Запуск ядра программы
    m_engine = new Engine();
    //Запуск сетевого клиента
    m_netClient = new NetWorkClient(this);

    //Запуск маппера модель-поля ввода
    QDataWidgetMapper *mapper = new QDataWidgetMapper;
    mapper->setModel( m_engine->model() );
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    //получим размер экрана
    desktop_avail = desktop.availableGeometry(desktop.primaryScreen());

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);    
    this->setMaximumSize(desktop_avail.width()-5,desktop_avail.height()-5);
    this->move( calcDeskTopCenter(this->width(),this->height() ));

    mainLayout = new QVBoxLayout(this);
    //-------------------------------- Центральный блок -----------------------

    QGroupBox *groupBox = new QGroupBox();
    QGridLayout gridLayout = new QGridLayout(groupBox);

    QLabel *label   = new QLabel(groupBox);
    QLabel *label_0 = new QLabel(groupBox);
    QLabel *label_1 = new QLabel(groupBox);
    QLabel *label_2 = new QLabel(groupBox);

    label->setText  (QObject::trUtf8("Логин:"));
    label_0->setText(QObject::trUtf8("Мандат:"));
    label_1->setText(QObject::trUtf8("Имя ZP запроса:"));
    label_2->setText(QObject::trUtf8("Имя групой машины:"));


    NBMlineEdit    = new QLineEdit(groupBox);
    ReqlineEdit    = new QLineEdit(groupBox);

    // Валидаторы
    QRegExp rxInt( "^[0-9]{1,3}$" );
    QRegExp rxReq( "^"
                  "([0-9]{1,3}[,\\-,\\,]{1}){1,}"
                  "$");
    QRegExpValidator *validator_INT_3 = new QRegExpValidator( rxInt, 0 );
    QRegExpValidator *validator_Req = new QRegExpValidator( rxReq, 0 );

    //ReqlineEdit->setValidator( validator_Req );


    formLayout->setWidget(0, QFormLayout::LabelRole, label);
    formLayout->setWidget(1, QFormLayout::LabelRole, label_0);
    formLayout->setWidget(2, QFormLayout::LabelRole, label_1);
    formLayout->setWidget(3, QFormLayout::LabelRole, label_2);
    formLayout->setWidget(4, QFormLayout::LabelRole, label_3);
    formLayout->setWidget(5, QFormLayout::LabelRole, label_4);
    formLayout->setWidget(6, QFormLayout::LabelRole, label_5);
    formLayout->setWidget(7, QFormLayout::LabelRole, label_6);

    formLayout->setWidget(0, QFormLayout::FieldRole, LoginlineEdit);
    formLayout->setWidget(1, QFormLayout::FieldRole, MandatlineEdit);
    formLayout->setWidget(2, QFormLayout::FieldRole, NameZPlineEdit);
    formLayout->setWidget(3, QFormLayout::FieldRole, NameGMlineEdit);
    formLayout->setWidget(4, QFormLayout::FieldRole, DayMlineEdit);
    formLayout->setWidget(5, QFormLayout::FieldRole, VRNTlineEdit);
    formLayout->setWidget(6, QFormLayout::FieldRole, NBMlineEdit);
    formLayout->setWidget(7, QFormLayout::FieldRole, ReqlineEdit);
    formLayout->setWidget(8, QFormLayout::LabelRole, TracecheckBox);

    //-------------------------------- Модная линия ---------------------------
    line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QGroupBox *led_groupBox = new QGroupBox();
    QFormLayout *formLayout_2 = new QFormLayout(led_groupBox);

    QLabel *led_label = new QLabel(led_groupBox);
    QLabel *led_label_1 = new QLabel(led_groupBox);    

    led_label->setText  (QObject::trUtf8("Авторизация пользователя:"));
    led_label_1->setText(QObject::trUtf8("Связь с Упр.Сервером:"));

    led_auth = new QLedIndicator(this);
    led_surd = new QLedIndicator(this);

    led_auth->setEnabled(false);
    led_surd->setEnabled(false);    

    led_auth->setToolTip(QObject::trUtf8("Авторизация пользователя"));
    led_surd->setToolTip(QObject::trUtf8("Связь с СУРД"));

    formLayout_2->setWidget(0, QFormLayout::LabelRole, led_label);
    formLayout_2->setWidget(1, QFormLayout::LabelRole, led_label_1);

    formLayout_2->setWidget(0,QFormLayout::FieldRole,led_auth);
    formLayout_2->setWidget(1,QFormLayout::FieldRole,led_surd);    
    led_groupBox->setTitle(QObject::trUtf8("Состояние программы"));
    //------------------------------- Нижний блок - Кнопки --------------------

    horizontalLayout = new QHBoxLayout();
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding,
                                       QSizePolicy::Minimum);
    horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding,
                                         QSizePolicy::Minimum);
    helpButton       = new QPushButton(this);    
    sendButton       = new QPushButton(this);
    exitButton       = new QPushButton(this);

    helpButton->setText(QObject::trUtf8("Справка"));
    sendButton->setText(QObject::trUtf8("Записать"));
    exitButton->setText(QObject::trUtf8("Выход"));

    sendButton->setDefault(true);
    sendButton->setEnabled(true);

    horizontalLayout->addWidget(helpButton);
    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(sendButton);
    horizontalLayout->addItem(horizontalSpacer_2);
    horizontalLayout->addWidget(exitButton);
    //-------------------------------------- Компоновка -----------------------

    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(line);
    mainLayout->addWidget(led_groupBox);
    mainLayout->addLayout(horizontalLayout);

    connect(exitButton,SIGNAL(clicked()),qApp,SLOT(quit()));

    connect(sendButton,SIGNAL(clicked()),mapper,SLOT(submit()));
    connect(sendButton,SIGNAL(clicked()),this,SLOT(sendDataToNet()));

    connect(m_engine,SIGNAL(error(VPrn::AppErrorType,QString)),
            this,    SLOT (do_error(VPrn::AppErrorType,QString))
            );
    connect(m_engine, SIGNAL(definedAuthData(const QString&,const QString&)),
            this,     SLOT  (checkAuthUser(const QString&,const QString&))
            );

    connect (m_netClient, SIGNAL(reciveMessageString(QString)),
             this       , SLOT  (parseNetworkMessage(QString))
             );
    connect (m_netClient, SIGNAL(connectToRemoteServer()),
             this       , SLOT  (networkEnabled())
             );

    connect(LoginlineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkDataFill())
            );
    connect(MandatlineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkDataFill())
            );
    connect(NameZPlineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkDataFill())
            );
    connect(NameGMlineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkDataFill())
            );
    /*
    connect(DayMlineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkDataFill())
            );

    connect(VRNTlineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkDataFill())
            );

    connect(NBMlineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkDataFill())
            );
    */
    connect(ReqlineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkDataFill())
            );
    connect(TracecheckBox, SIGNAL(clicked(bool)),
            m_engine, SLOT(saveTraceState(bool))
            );

    // свяжем модель данных и поля ввода данных
    mapper->addMapping(LoginlineEdit, 0);
    mapper->addMapping(MandatlineEdit, 1);
    mapper->addMapping(NameZPlineEdit, 2);
    mapper->addMapping(NameGMlineEdit,3);
    mapper->addMapping(DayMlineEdit,4);
    mapper->addMapping(VRNTlineEdit,5);
    mapper->addMapping(NBMlineEdit,6);
    mapper->addMapping(ReqlineEdit,7);
    //mapper->addMapping(TracecheckBox,8);
    mapper->toFirst();
    // Грязный хак
    TracecheckBox->click();
}

void MainWindow::init(const QString &app_dir)
{

    if (readConfig(app_dir)){
        // Запуск сети
        m_netClient->connectToHost(serverName,serverPort);

    }    
}

void MainWindow::appendStartMsg(const QString &txt_msg)
{
    myEMsgBox->showMessage(txt_msg);
}

MainWindow::~MainWindow()
{
}

//------------------------------------PRIVATE SLOTS ------------------------------------

void MainWindow::checkAuthUser(const QString &login, const QString &mandat)
{
    if (!login.isEmpty()){

        LoginlineEdit->setText(login);

        if (!mandat.isEmpty()){
            MandatlineEdit->setText(mandat);
            led_auth->setChecked(true);
        }else{
            // Запрос списка доступных мандатов из сети
            // Формат VPrn::Que_MANDAT_LIST;:;login
            QString msg= QString("BPUSK_PLAIN %1;:;%2").arg(VPrn::Que_MANDAT_LIST,0,10).arg(login);
            m_netClient->reSendNetworkMessage(msg);
        }
    }

}

void MainWindow::do_needAuthUser(const QString &login_mandat_list)
{

}

void MainWindow::do_error(VPrn::AppErrorType eCode,QString e_msg)
{
    QString extMsg = QString("eCode %1. AppsError:%2")
            .arg(eCode,0,10).arg(e_msg) ;
    myEMsgBox->showMessage(extMsg);
    qDebug() << extMsg;
}

void MainWindow::parseNetworkMessage(const QString & msg)
{
    // myEMsgBox->showMessage(msg);

    // Разберем тело ответа на части [кому];:;что_передали
    QRegExp rx("\\[(.+)\\];:;(.*)");
    //rx.setMinimal(true);

    if(rx.indexIn(msg) != -1){
        int cmd   = rx.cap(1).toInt();
        QString m_body  = rx.cap(2);
        switch (cmd){
        case VPrn::Ans_MANDAT_LIST:
            this->do_needAuthUser(m_body);
            break;

        }
    }else{
        myEMsgBox->showMessage(QObject::trUtf8("Ошибка разбора сообщения сервера."));
    }
}

void MainWindow::networkEnabled()
{
    led_surd->setChecked(true);
}

void MainWindow::checkDataFill()
{
    bool ok =true;
    {

        if ( LoginlineEdit->text().isEmpty()){
            ok &=false;
        }
        if ( MandatlineEdit->text().isEmpty()){
            ok &=false;
        }
        if ( NameZPlineEdit->text().isEmpty()){
            ok &=false;
        }
        if ( NameGMlineEdit->text().isEmpty()){
            ok &=false;
        }else{

        }
        /*
        if ( DayMlineEdit->text().isEmpty()){
            ok &=false;
        }

        if ( VRNTlineEdit->text().isEmpty()){
            ok &=false;
        }

        if ( NBMlineEdit->text().isEmpty()){
            ok &=false;
        }
        */
        if ( ReqlineEdit->text().isEmpty()){
            ok &=false;
        }

    }

    //sendButton->setEnabled(ok);    
}

void MainWindow::sendDataToNet()
{
    /*
    m_engine->checkingModel();
    QStringList s_data;
    m_engine->modelToStringList(s_data);

    for (int i=0;i<s_data.count();i++){
        m_netClient->reSendNetworkMessage(s_data.at(i));
    }
    */
}


//------------------------------------ PRIVATE ---------------------------------------------

QPoint MainWindow::calcDeskTopCenter(int width,int height)
{
    QPoint centerWindow;
    //получаем прямоугольник с размерами как у экрана
    centerWindow = desktop_avail.center(); //получаем координаты центра экрана
    centerWindow.setX(centerWindow.x() - (width/2));
    centerWindow.setY(centerWindow.y() - (height/2));
    return centerWindow;
}

bool MainWindow::readConfig(const QString &app_dir)
{
    QString ini_file =QString("%1/Zarya.ini").arg(app_dir);
    if (QFile::exists(ini_file)){
        QSettings settings (ini_file,QSettings::IniFormat);

        settings.beginGroup("SERVICE");
        serverName      = settings.value("server").toString();
        serverPort      = settings.value("port").toInt();
        settings.endGroup();
        settings.beginGroup("FILES_AND_FOLDERS");
        //m_gmlist_file      = settings.value("gmlist").toString();
        settings.endGroup();
        settings.beginGroup("USER_DEFAULT");
        //m_gm      = settings.value("gm").toString();
        settings.endGroup();
    }else{
        myEMsgBox->showMessage(QObject::trUtf8("Файл с настройками программы %1 не найден!\n%2")
                               .arg(ini_file)
                               .arg(QString(Q_FUNC_INFO))
                               );
    }
    if (!serverName.isEmpty() && !serverPort !=-1 ){
        return true;
    }else{
        return false;
    }
}