#include "editcardpage.h"
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QRadioButton>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLineEdit>
#include <QtGui/QFont>
#include <QtGui/QSpinBox>
#include <QtGui/QDateEdit>
#include <QtGui/QMessageBox>

#include <QSizePolicy>
#include <QPushButton>
#include <QStandardItemModel>
#include <QDataWidgetMapper>


EditCardPage::EditCardPage(QWidget *parent)
        : QWidget(parent)
        , cardValid(false)
{
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
    this->setWindowTitle(QObject::trUtf8("Заполнение карточки документа"));

    QGridLayout *centralgridLayout = new QGridLayout(this);

    //--------------------- doc name -------------------------------------------
    QGroupBox *centralGroupBox = new QGroupBox(QObject::trUtf8("Название документа"),this);
    QHBoxLayout *hBoxLayout = new QHBoxLayout(centralGroupBox);

    docName_lineEd = new QLineEdit(centralGroupBox);
    docName_lineEd->setEnabled(true);

    hBoxLayout->addWidget(docName_lineEd);
    //--------------------- common atrib --------------------------------------

    QGroupBox *commonGroupBox = new QGroupBox(QObject::trUtf8("Общие атрибуты"),this);
    //commonGroupBox->setEnabled(true);
    QGridLayout *gridLayout = new QGridLayout(commonGroupBox);
    QLabel *label    = new QLabel(QObject::trUtf8("Гриф"),commonGroupBox);
    QLabel *label_3  = new QLabel(QObject::trUtf8("Пункт"),commonGroupBox);
    QLabel *label_4  = new QLabel(QObject::trUtf8("Номер МБ"),commonGroupBox);
    QLabel *label_15 = new QLabel(QObject::trUtf8("Количество листов"),commonGroupBox);
    QLabel *label_10 = new QLabel(QObject::trUtf8("Принтеры:"),commonGroupBox);
    QLabel *label_tmplCB = new QLabel(QObject::trUtf8("Шаблоны"),commonGroupBox);

    secretCBox = new QComboBox(commonGroupBox);
    punktLineEd = new QLineEdit(commonGroupBox);
    mbNumberLineEd = new QLineEdit(commonGroupBox);
    pagesCountLineEd = new QLineEdit(commonGroupBox                                     );
    printersCBox = new QComboBox(commonGroupBox);
    templatesCBox = new QComboBox(commonGroupBox);

    pagesCountLineEd->setReadOnly(true);
    //pagesCountLineEd->setEnabled(false);

    gridLayout->addWidget(label,            0, 0, 1, 1);
    gridLayout->addWidget(secretCBox,       0, 1, 1, 9);
    gridLayout->addWidget(label_4,          1, 0, 1, 3);
    gridLayout->addWidget(mbNumberLineEd,   1, 4, 1, 6);

    gridLayout->addWidget(label_3,          2, 0, 1, 1);
    gridLayout->addWidget(punktLineEd,      2, 4, 1, 6);
    gridLayout->addWidget(label_15,         3, 0, 1, 6);
    gridLayout->addWidget(pagesCountLineEd, 3, 8, 1, 2);

    gridLayout->addWidget(label_10,         4, 0, 1, 2);
    gridLayout->addWidget(printersCBox,     5, 0, 1, 10);
    gridLayout->addWidget(label_tmplCB,     6, 0, 1, 1);
    gridLayout->addWidget(templatesCBox,    7, 0, 1, 10);
    //--------------------- last page stamp -----------------------------------
    QGroupBox *lastPageGroupBoxChecked = new QGroupBox(QObject::trUtf8("Штамп последней страницы"),this);
    QGridLayout *gridLayout_2 = new QGridLayout(lastPageGroupBoxChecked);

    QLabel *label_5 = new QLabel(QObject::trUtf8("Исполнитель"),lastPageGroupBoxChecked);
    QLabel *label_6 = new QLabel(QObject::trUtf8("Отпечатал"),lastPageGroupBoxChecked);
    QLabel *label_7 = new QLabel(QObject::trUtf8("Телефон"),lastPageGroupBoxChecked);
    QLabel *label_8 = new QLabel(QObject::trUtf8("Инв. №"),lastPageGroupBoxChecked);
    QLabel *label_9 = new QLabel(QObject::trUtf8("Дата"),lastPageGroupBoxChecked);

    executor_lineEd = new QLineEdit(lastPageGroupBoxChecked);    
    pressman_lineEd = new QLineEdit(lastPageGroupBoxChecked);
    invNumber_lineEd = new QLineEdit(lastPageGroupBoxChecked);
    telephone_lineEd = new QLineEdit(lastPageGroupBoxChecked);
    dateField_dateEd = new QDateEdit(lastPageGroupBoxChecked);


    pressman_lineEd->setEnabled(true);
    executor_lineEd->setEnabled(true);
    invNumber_lineEd->setEnabled(true);
    telephone_lineEd->setEnabled(true);
    dateField_dateEd->setEnabled(true);
    dateField_dateEd->setDateTime(QDateTime::currentDateTime());
    dateField_dateEd->setCalendarPopup(true);

    gridLayout_2->addWidget(label_5,          0, 0, 1, 1);
    gridLayout_2->addWidget(executor_lineEd,  0, 1, 1, 1);
    gridLayout_2->addWidget(label_6,          1, 0, 1, 1);
    gridLayout_2->addWidget(pressman_lineEd,  1, 1, 1, 1);
    gridLayout_2->addWidget(label_7,          2, 0, 1, 1);
    gridLayout_2->addWidget(telephone_lineEd, 2, 1, 1, 1);
    gridLayout_2->addWidget(label_8,          3, 0, 1, 1);
    gridLayout_2->addWidget(invNumber_lineEd, 3, 1, 1, 1);
    gridLayout_2->addWidget(label_9,          5, 0, 1, 1);
    gridLayout_2->addWidget(dateField_dateEd, 5, 1, 1, 1);

    //---------------- reciver list
    QGroupBox *reciversListGroupBoxChecked = new QGroupBox(QObject::trUtf8("Список рассылки"),this);
    QGridLayout *gridLayout_4 = new QGridLayout(reciversListGroupBoxChecked);

    reciver_lineEd_1 = new QLineEdit(reciversListGroupBoxChecked);
    reciver_lineEd_2 = new QLineEdit(reciversListGroupBoxChecked);
    reciver_lineEd_3 = new QLineEdit(reciversListGroupBoxChecked);
    reciver_lineEd_4 = new QLineEdit(reciversListGroupBoxChecked);
    reciver_lineEd_5 = new QLineEdit(reciversListGroupBoxChecked);

    reciver_lineEd_1->setEnabled(false);
    reciver_lineEd_2->setEnabled(false);
    reciver_lineEd_3->setEnabled(false);
    reciver_lineEd_4->setEnabled(false);
    reciver_lineEd_5->setEnabled(false);

    copy_checkBox_1 = new QCheckBox(QObject::trUtf8("Экземпляр №1"),reciversListGroupBoxChecked);
    copy_checkBox_2 = new QCheckBox(QObject::trUtf8("Экземпляр №2"),reciversListGroupBoxChecked);
    copy_checkBox_3 = new QCheckBox(QObject::trUtf8("Экземпляр №3"),reciversListGroupBoxChecked);
    copy_checkBox_4 = new QCheckBox(QObject::trUtf8("Экземпляр №4"),reciversListGroupBoxChecked);
    copy_checkBox_5 = new QCheckBox(QObject::trUtf8("Экземпляр №5"),reciversListGroupBoxChecked);

    gridLayout_4->addWidget(copy_checkBox_1, 0, 0, 1, 1);
    gridLayout_4->addWidget(copy_checkBox_2, 2, 0, 1, 1);
    gridLayout_4->addWidget(copy_checkBox_3, 3, 0, 1, 1);
    gridLayout_4->addWidget(copy_checkBox_4, 4, 0, 1, 1);
    gridLayout_4->addWidget(copy_checkBox_5, 5, 0, 1, 1);

    gridLayout_4->addWidget(reciver_lineEd_1, 0, 1, 1, 1);
    gridLayout_4->addWidget(reciver_lineEd_2, 2, 1, 1, 1);
    gridLayout_4->addWidget(reciver_lineEd_3, 3, 1, 1, 1);
    gridLayout_4->addWidget(reciver_lineEd_4, 4, 1, 1, 1);
    gridLayout_4->addWidget(reciver_lineEd_5, 5, 1, 1, 1);

    centralgridLayout->addWidget( centralGroupBox             ,0,0,1,3);
    centralgridLayout->addWidget( commonGroupBox              ,1,0,1,1);
    centralgridLayout->addWidget( lastPageGroupBoxChecked     ,1,1,1,1 );
    centralgridLayout->addWidget( reciversListGroupBoxChecked ,2,0,1,3 );

    /*
    docName_lineEd->setFocus();
    this->setTabOrder(docName_lineEd, secretCBox);
    this->setTabOrder(secretCBox,punktLineEd);
    this->setTabOrder(punktLineEd,mbNumberLineEd);
    this->setTabOrder(mbNumberLineEd,printersCBox);
    this->setTabOrder(printersCBox,templatesCBox);
*/
    setLayout(centralgridLayout);
    this->connect_all();
}



void EditCardPage::setCardModel(QStandardItemModel *cModel)
{
    //Настройка маппера
    mapper->setModel(cModel);

    mapper->addMapping(docName_lineEd,  VPrn::cards_DOC_NAME);
    mapper->addMapping(secretCBox,      VPrn::cards_STAMP, "currentIndex");
    mapper->addMapping(punktLineEd,     VPrn::cards_PUNKT);
    mapper->addMapping(mbNumberLineEd,  VPrn::cards_MB_NUMBER);
    mapper->addMapping(pagesCountLineEd,VPrn::cards_PAGE_COUNT);
    mapper->addMapping(printersCBox,    VPrn::cards_PRINTER_NAME,  "currentIndex");
    mapper->addMapping(templatesCBox,   VPrn::cards_TEMPLATE_NAME, "currentIndex");
    mapper->addMapping(executor_lineEd, VPrn::cards_EXECUTOR);
    mapper->addMapping(pressman_lineEd, VPrn::cards_PRINTMAN);
    mapper->addMapping(invNumber_lineEd,VPrn::cards_INV_NUMBER);
    mapper->addMapping(telephone_lineEd,VPrn::cards_PHONE);
    mapper->addMapping(dateField_dateEd,VPrn::cards_PRINT_DATE);
    mapper->addMapping(reciver_lineEd_1,VPrn::cards_RECIVER_1);
    mapper->addMapping(reciver_lineEd_2,VPrn::cards_RECIVER_2);
    mapper->addMapping(reciver_lineEd_3,VPrn::cards_RECIVER_3);
    mapper->addMapping(reciver_lineEd_4,VPrn::cards_RECIVER_4);
    mapper->addMapping(reciver_lineEd_5,VPrn::cards_RECIVER_5);


    // Грязный хак
    QObject::connect(cModel, SIGNAL(itemChanged(QStandardItem*)),
                     mapper, SLOT (toLast())
                     );
}


void EditCardPage::setStampModel(QStringListModel *sModel)
{
    secretCBox->setModel(sModel);
    secretCBox->setCurrentIndex(-1);
}

void EditCardPage::setPrinterModel(QStandardItemModel *pModel)
{
    printersCBox->setModel(pModel);
    /// @todo fix magic number
    printersCBox->setModelColumn(1);
    printersCBox->setCurrentIndex(-1);
}

void EditCardPage::setTemplatesModel(QStandardItemModel *tModel)
{
    templatesCBox->setModel(tModel);
    templatesCBox->setModelColumn(0);
    templatesCBox->setCurrentIndex(-1);
}

//------------------------- Public Slots --------------------------------------
void EditCardPage::setCardValid(bool flag)
{
    cardValid = flag;
}

//------------------------- Private slots -------------------------------------

void EditCardPage::checkNext()
{    

    bool Ok = true;
    {        
        Ok &= !docName_lineEd->text().isEmpty();
        Ok &= !executor_lineEd->text().isEmpty();

        if (pressman_lineEd->text().isEmpty()){
            pressman_lineEd->setText(executor_lineEd->text());
            Ok &= pressman_lineEd->isModified();
        }

        if (secretCBox->currentIndex() == -1){
            Ok &= false;
        }

        if (templatesCBox->currentIndex() == -1){
            Ok &= false;
        }

        Ok &= !mbNumberLineEd->text().isEmpty();
        Ok &= !pagesCountLineEd->text().isEmpty();
        Ok &= !invNumber_lineEd->text().isEmpty();
        Ok &= !telephone_lineEd->text().isEmpty();
        Ok &= !dateField_dateEd->text().isEmpty();
        /// @todo Дописать проверку на поля отправителей. Проверку по БД

    }
    if (Ok){
        emit enableNext();
        this->setCardValid(Ok);
    }

}

//------------------------- Private -------------------------------------------
void EditCardPage::connect_all()
{
    connect(copy_checkBox_1,SIGNAL(toggled(bool)),
            reciver_lineEd_1,SLOT(setEnabled(bool))
            );
    connect(copy_checkBox_2,SIGNAL(toggled(bool)),
            reciver_lineEd_2,SLOT(setEnabled(bool))
            );
    connect(copy_checkBox_3,SIGNAL(toggled(bool)),
            reciver_lineEd_3,SLOT(setEnabled(bool))
            );
    connect(copy_checkBox_4,SIGNAL(toggled(bool)),
            reciver_lineEd_4,SLOT(setEnabled(bool))
            );
    connect(copy_checkBox_5,SIGNAL(toggled(bool)),
            reciver_lineEd_5,SLOT(setEnabled(bool))
            );

    connect(docName_lineEd,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );
    connect(punktLineEd,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );
    connect(mbNumberLineEd,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );
    connect(executor_lineEd,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );
    connect(pressman_lineEd,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );
    connect(invNumber_lineEd,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );
    connect(pagesCountLineEd,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );
    connect(telephone_lineEd,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );
    connect(reciver_lineEd_1,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );
    connect(reciver_lineEd_2,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );
    connect(reciver_lineEd_3,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );
    connect(reciver_lineEd_4,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );
    connect(reciver_lineEd_5,SIGNAL(editingFinished()),
            this,SLOT(checkNext())
            );

    connect(secretCBox,SIGNAL(currentIndexChanged(int)),
            this,SLOT(checkNext())
            );

    connect(templatesCBox,SIGNAL(currentIndexChanged(int)),
            this,SLOT(checkNext())
            );
    connect(printersCBox,SIGNAL(currentIndexChanged(int)),
            this,SLOT(checkNext())
            );

}

