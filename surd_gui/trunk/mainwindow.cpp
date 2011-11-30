#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QSettings>

#include <QtGui/QCheckBox>
#include <QtGui/QApplication>

#include <QtGui/QFrame>

#include <QtGui/QFormLayout>
#include <QtGui/QVBoxLayout>

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>

#include <QtGui/QWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QGroupBox>
#include <QtGui/QListWidget>

#include <QFont>
#include <QRegExp>
#include <QRegExpValidator>

#include <QtGui/QErrorMessage>
#include <QComboBox>
#include <QTreeView>

#include <QMessageBox>
#include <QHeaderView>
#include "mainwindow.h"
#include "editprinterproperty.h"



using namespace VPrn;

MainWindow::MainWindow(QWidget *parent)
        : QWidget(parent)
        , localdb(0)
        , engine(0)
        , serverName(QString())
        , serverPort(-1)

{
    this->setWindowTitle(QObject::trUtf8("Управление принтерами в системе ЗАРЯ"));
    //Основной потребитель ошибок
    myEMsgBox = new QErrorMessage(this);
    // Запуск ядра программы
    engine = new Engine();
    // Запуск loaclDb
    localdb = new LocalDB();

    //получим размер экрана
    desktop_avail = desktop.availableGeometry(desktop.primaryScreen());

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);    
    this->setMaximumSize(desktop_avail.width()-5,desktop_avail.height()-5);
    this->move( calcDeskTopCenter(this->width(),this->height() ));

    mainLayout = new QVBoxLayout(this);
    //-------------------------------- Центральный блок -----------------------
    prnTreeView = new QTreeView(this);

    prnTreeView->setModel( localdb->getDatamodel() );
    prnTreeView->header()->setStretchLastSection(false);

    prnTreeView->header()->setResizeMode(0, QHeaderView::Stretch);
    prnTreeView->header()->setResizeMode(1, QHeaderView::Interactive);    

    prnTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    prnTreeView->setColumnHidden(2,true);

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
    led_net = new QLedIndicator(this);

    led_auth->setEnabled(false);
    led_net->setEnabled(false);

    led_auth->setToolTip(QObject::trUtf8("Авторизация пользователя"));
    led_net->setToolTip(QObject::trUtf8("Связь с СУРД"));

    formLayout_2->setWidget(0, QFormLayout::LabelRole, led_label);
    formLayout_2->setWidget(1, QFormLayout::LabelRole, led_label_1);

    formLayout_2->setWidget(0,QFormLayout::FieldRole,led_auth);
    formLayout_2->setWidget(1,QFormLayout::FieldRole,led_net);
    led_groupBox->setTitle(QObject::trUtf8("Состояние программы"));
    //------------------------------- Нижний блок - Кнопки --------------------

    horizontalLayout = new QHBoxLayout();
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding,
                                       QSizePolicy::Minimum);
    horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding,
                                         QSizePolicy::Minimum);
    helpButton       = new QPushButton(this);    
    editButton       = new QPushButton(this);
    exitButton       = new QPushButton(this);

    helpButton->setText(QObject::trUtf8("Справка"));
    editButton->setText(QObject::trUtf8("Редактировать"));
    exitButton->setText(QObject::trUtf8("Выход"));

    editButton->setDefault(true);
    editButton->setEnabled(false);

    horizontalLayout->addWidget(helpButton);
    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget (editButton);
    horizontalLayout->addItem(horizontalSpacer_2);
    horizontalLayout->addWidget(exitButton);
    //-------------------------------------- Компоновка -----------------------

    mainLayout->addWidget(prnTreeView);
    mainLayout->addWidget(line);
    mainLayout->addWidget(led_groupBox);
    mainLayout->addLayout(horizontalLayout);

    this->connectAll();

}

void MainWindow::init(const QString &app_dir)
{
    if (readConfig(app_dir) && engine){
        engine->setServer(serverName,serverPort);
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

void MainWindow::netLedEnabled()
{
    led_net->setChecked(true);
}

void MainWindow::authLedEnabled()
{
    led_auth->setChecked(true);
}

void MainWindow::do_error(VPrn::AppErrorType eCode,QString e_msg)
{
    QString extMsg = QString("eCode %1. AppsError:%2")
                     .arg(eCode,0,10).arg(e_msg) ;
    myEMsgBox->showMessage(extMsg);
    qDebug() << extMsg;
}

void MainWindow::editSelectItem()
{

    QModelIndex index = prnTreeView->currentIndex();

    QString prn =localdb->getPrinterNameFromIndex(&index);
    QString mandat = localdb->getPrinterMandatFromIndex(&index);
    qDebug() << prn << mandat;

    int i_row = index.row();
    QModelIndex idx = localdb->getDatamodel()->index(i_row,0);
    if ( idx.parent() != QModelIndex()){
        idx = idx.parent();
    }

    // Формируем Окно редактора
    EditPrinterProperty *editDlg = new EditPrinterProperty(this);
    editDlg->setPrinterAtrib(prn,mandat);
    editDlg->setVisiblePartModel(localdb->getDatamodel(),idx);
    //editDlg->setPrinterAtrib();
    editDlg->exec();
/*
    QMessageBox msgBox;
    msgBox.setText(item->data(Qt::DisplayRole).toString());
    msgBox.exec();
    */
}

void MainWindow::help()
{
}
void MainWindow::enableEditMode()
{
    editButton->setEnabled(led_net->isChecked() && led_auth->isChecked());
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
    QString ini_file =QString("%1/zarya.ini").arg(app_dir);
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

void MainWindow::connectAll()
{
    // ---------------------------- Buttons
    connect(helpButton,SIGNAL(clicked()),this,SLOT(help()));
    connect(editButton,SIGNAL(clicked()),this,SLOT(editSelectItem()));
    connect(editButton,SIGNAL(clicked()),this,SLOT(sendDataToNet()));
    connect(exitButton,SIGNAL(clicked()),qApp,SLOT(quit()));
    //----------------------------- Engine
    connect(engine,SIGNAL(error(VPrn::AppErrorType,QString)),
            this,    SLOT (do_error(VPrn::AppErrorType,QString))
            );

    connect(engine, SIGNAL(enableEditMode(bool) ),
            this,     SLOT  (enableEditMode(bool) )
            );
    connect(engine, SIGNAL(connected() ),
            this,     SLOT  (netLedEnabled() )
            );
    connect(engine, SIGNAL(authUserPass() ),
            this,     SLOT  (authLedEnabled() )
            );

    connect(engine, SIGNAL (saveBaseSlice(QByteArray*)) ,
            localdb, SLOT  (restoreDump(QByteArray *) )
            );

    connect( prnTreeView, SIGNAL( doubleClicked(const QModelIndex&) ),
             this, SLOT( editSelectItem(const QModelIndex&) ) );
    //----------------------------------Led
    connect(led_auth, SIGNAL(toggled(bool)),
            this, SLOT(enableEditMode())
            );

    connect(led_net, SIGNAL(toggled(bool)),
            this, SLOT(enableEditMode())
            );
    //--------------------------------- prnTreeView

}


