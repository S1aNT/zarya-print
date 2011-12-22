#include "selectway.h"
#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSpacerItem>

//:QWidget(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
SelectWay::SelectWay(QWidget *parent)
    :QDialog(parent)
{

    this->setFixedSize(240,240);
    setWindowTitle(QObject::trUtf8("Выберите..."));

    QVBoxLayout *mainLayout = new QVBoxLayout;

//    QFrame *line = new QFrame(this);
//    line->setFrameShape(QFrame::HLine);
//    line->setFrameShadow(QFrame::Sunken);

    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem *horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   // QSpacerItem *horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);


    printButton       = new QPushButton(QObject::trUtf8("Печать документа"));
    previewAllButton  = new QPushButton(QObject::trUtf8("Предпросмотр всего документа"));
    previewPartButton = new QPushButton(QObject::trUtf8("Предпросмотр части документа"));
    //contEditButton    = new QPushButton(QObject::trUtf8("Отмена"));

    printButton->setDefault(true);
    previewAllButton->setAutoDefault(false);
    previewPartButton->setAutoDefault(false);
    //contEditButton->setAutoDefault(false);


    mainLayout->addWidget(printButton);
    mainLayout->addItem(horizontalSpacer);
    //mainLayout->addWidget(line);
    mainLayout->addWidget(previewAllButton);
    mainLayout->addItem(horizontalSpacer_2);
    mainLayout->addWidget(previewPartButton);
    //mai->addItem(horizontalSpacer_3);

    //mainLayout->addWidget(contEditButton);

    setLayout(mainLayout);

    signalMapper = new QSignalMapper(this);
    signalMapper->setMapping(printButton,1);
    signalMapper->setMapping(previewAllButton,2);
    signalMapper->setMapping(previewPartButton,3);
    //signalMapper->setMapping(contEditButton,4);

    connect(printButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(previewAllButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(previewPartButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
    //connect(contEditButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

    connect(signalMapper,SIGNAL(mapped(int )),this, SLOT(selectButton(int)));


//    connect(printButton,SIGNAL(clicked()),this,SIGNAL(selPrint()));
//    connect(previewAllButton,SIGNAL(clicked()),this,SIGNAL(selPreView()));
//    connect(previewPartButton,SIGNAL(clicked()),this,SIGNAL(selPartPreView()));
//    connect(contEditButton,SIGNAL(clicked()),this,SLOT(reject()));
}

void SelectWay::selectButton(int btn)
{
    switch (btn){
    case 1:
        emit selPrint();
        break;
    case 2:
        emit selPreView();
        break;
    case 3:
        emit selPartPreView();
        break;
    default:
        break;
    }
    this->accept();
    this->close();
}
