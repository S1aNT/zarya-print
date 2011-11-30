#include <QErrorMessage>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QTimer>
#include <QTableView>
#include <QMessageBox>

#include "mainwindow.h"
#include "infowindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
    this->setWindowTitle(QObject::trUtf8("Управление принтерами в системе ЗАРЯ"));
    //Список окошек
    introPage    = new IntroPage();
    editCardPage = new EditCardPage();
    preViewPage  = new PreViewPage();
    //Основной потребитель ошибок
    myEMsgBox = new QErrorMessage(this);
    eLogger = new ELogger();

    // Запуск ядра программы
    controller = new Controller();

    //получим размер экрана
    desktop_avail = desktop.availableGeometry(desktop.primaryScreen());

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
    //this->setMinimumSize(320,240);
    this->setMaximumSize(desktop_avail.width()-5,desktop_avail.height()-5);
    this->move( calcDeskTopCenter(this->width(),this->height() ));
    //-------------------- QStackedWidget --------------------------------------
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    stackedWidget = new QStackedWidget();

    stackedWidget->addWidget(introPage);
    stackedWidget->addWidget(editCardPage);
    stackedWidget->addWidget(preViewPage);


    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    //------------------------------- Нижний блок - Кнопки -------------------------------

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem *horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    helpButton       = new QPushButton(this);
    prevButton       = new QPushButton(this);
    nextButton       = new QPushButton(this);
    //restartButton    = new QPushButton(this);
    exitButton       = new QPushButton(this);

    helpButton->setText(QObject::trUtf8("Справка"));
    prevButton->setText(QObject::trUtf8("< Назад"));
    nextButton->setText(QObject::trUtf8("Вперед >"));
    //restartButton->setText(QObject::trUtf8("В начало"));
    exitButton->setText(QObject::trUtf8("Выход"));

    prevButton->setEnabled(false);
    nextButton->setEnabled(false);
    //restartButton->setEnabled(false);

    horizontalLayout->addWidget(helpButton);
    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(prevButton);
    horizontalLayout->addWidget(nextButton);
    //horizontalLayout->addWidget(restartButton);
    horizontalLayout->addItem(horizontalSpacer_2);
    horizontalLayout->addWidget(exitButton);
    //-------------------------------------- Компоновка ----------------------------------------------------

    mainLayout->addWidget(stackedWidget);
    mainLayout->addWidget(line);
    mainLayout->addLayout(horizontalLayout);

    if (stackedWidget->currentIndex () != -1 ){
        this->setWindowTitle(stackedWidget->currentWidget()->windowTitle() );
    }else{
        this->setWindowTitle( QObject::trUtf8("Ошибка создания окон мастера") );
    }

    this->connectAll();

    editCardPage->setPrinterModel( controller->getPrinterModel() );
    editCardPage->setCardModel(controller->getCardModel());
    editCardPage->setTemplatesModel(controller->getTemplatesModel());
    editCardPage->setStampModel(controller->getStampListModel());

    preViewPage->setModel(controller->getPictureListModel());

    //    QTableView *t = new QTableView();
    //    t->setModel(controller->getCardModel());
    //    t->show();
}

void MainWindow::init(const QString &app_dir,const QString &wFile)
{
    if (!controller->init(app_dir,wFile)){
        myEMsgBox->showMessage(QObject::trUtf8("Ошибка инициализации подсистем.Приложение будет закрыто."));
        QTimer::singleShot(5000,qApp,SLOT(quit()));
    }

}

void MainWindow::appendStartMsg(const QString &txt_msg)
{
    myEMsgBox->showMessage(txt_msg);
}

MainWindow::~MainWindow()
{
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


void MainWindow::connectAll()        
{
    // ---------------------------- Buttons
    connect (helpButton,SIGNAL(clicked()),
             this,SLOT(help())
             );
    connect (exitButton,SIGNAL(clicked()),
             qApp,SLOT(quit())
             );
    connect (nextButton,SIGNAL(clicked()),
             this,      SLOT(do_next())
             );
    connect (prevButton,SIGNAL(clicked()),
             this,      SLOT(do_prev())
             );
    //    connect (restartButton,SIGNAL(clicked()),
    //             this,      SLOT(do_restart())
    //             );

    connect (introPage,SIGNAL(enableNext()),
             this, SLOT(enableNext())
             );

    connect (editCardPage,SIGNAL(enableNext()),
             this, SLOT(enableNext())
             );

    connect (controller,SIGNAL(setCardValid(bool)),
             editCardPage,SLOT(setCardValid(bool))
             );

    connect (controller,SIGNAL(finishStep(VPrn::InitSteps)),
             introPage,SLOT(setStep(VPrn::InitSteps))
             );
    connect (controller,SIGNAL(sendEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory)),
             eLogger,SLOT(saveEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory))
             );
    connect (controller,SIGNAL(sendEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory)),
             myEMsgBox,SLOT(showMessage(QString))
             );
    connect(controller,SIGNAL(needUpdatePicturesList()),
            preViewPage,SLOT(updatePictures())
            );
    connect(preViewPage,SIGNAL(enablePrintDoc()),
            this,SLOT(enableNext())
            );
    connect(controller,SIGNAL(serverInfo(QString,QString)),
            this,SLOT(showInfo(QString,QString))
            );

}



//------------------------------------------------ Private slots -------------------------------------------------
void MainWindow::help()
{

}

void MainWindow::do_next()
{    
    int pos = stackedWidget->currentIndex();

    if (!nextButton->isEnabled()){
        return;
    }

    switch (pos){
    case VPrn::page_EditCard:
        {
            SelectWay * sWay = new SelectWay();

            connect(sWay,SIGNAL(selPrint()),controller,SLOT(beginPrintCurrentDoc()));
#ifdef D_MYDEBUG
            connect(sWay,SIGNAL(selPrint()),this,SLOT(fakePrint()));
#endif
            connect(sWay,SIGNAL(selPreView()),controller,SLOT(genFullPreView()));
            connect(sWay,SIGNAL(selPartPreView()),controller,SLOT(genPartPreView()));

            if (sWay->exec() == QDialog::Accepted){
                stackedWidget->setCurrentIndex(VPrn::page_Preview);
                this->nextButton->setText(QObject::trUtf8("Печать"));
            }

        }

        break;
    case VPrn::page_Preview:
        controller->beginPrintCurrentDoc();
#ifdef D_MYDEBUG
        this->fakePrint();
#endif
        break;
    default:        
        break;
    }

    pos++;
    if (pos > stackedWidget->count()){
        return;
    }
    nextButton->setEnabled(false);
    stackedWidget->setCurrentIndex(pos);
}

void MainWindow::do_prev()
{
    int pos = stackedWidget->currentIndex();
    pos--;
    switch (pos){
    case VPrn::page_Preview:
        nextButton->setText(QObject::trUtf8("Вперед >"));
        controller->clearPreViewFiles();
        stackedWidget->setCurrentIndex(pos);
        break;
    default:
        break;
    }
}

void MainWindow::do_restart()
{
}

void MainWindow::enableNext()
{
    /// @todo Подумать может можно передавать собыить напрямую в кнопку
    nextButton->setEnabled(true);
}



void MainWindow::showInfo(const QString &title,const QString &txt)
{
    InfoWindow *info = new InfoWindow();
    info->setWindowTitle(title);
    info->setText(txt);
    info->exec();
}

#ifdef D_MYDEBUG
void MainWindow::fakePrint()
{
   QMessageBox msgbox;
   msgbox.setText(QObject::trUtf8("Документ успешно распечатан."));
   msgbox.exec();
}
#endif

