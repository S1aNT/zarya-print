#include "editprinterproperty.h"
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QRegExp>
#include <QTreeView>
#include <QLabel>
#include <QFont>

EditPrinterProperty::EditPrinterProperty(QWidget *parent) :
    QDialog(parent)
{
    QFont font;
    font.setFamily(QString::fromUtf8("Verdana"));
    font.setPointSize(10);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QGroupBox *groupBox = new QGroupBox();
    QFormLayout * formLayout = new QFormLayout(groupBox);
    printerLineEdit = new QLineEdit(groupBox);
    secLabelLineEdit  = new QLineEdit(groupBox);
    usersList = new QTreeView(groupBox);


    QLabel *label   = new QLabel(groupBox);
    QLabel *label_0 = new QLabel(groupBox);
    QLabel *label_1 = new QLabel(groupBox);

    label->setFont(font);
    label_0->setFont(font);
    label_1->setFont(font);

    label->setText  (QObject::trUtf8("Текущий принтер:"));
    label_0->setText(QObject::trUtf8("Метка конфиденциальности:"));
    label_1->setText(QObject::trUtf8("Список пользователей:"));

    // Валидаторы
    QRegExp rxInt( "^S[0-9]{1,2}\\:C[0-9]{1,4}$" );
    //QRegExpValidator *validator_SecLabel = ;

    secLabelLineEdit->setValidator( new QRegExpValidator( rxInt, 0 ) );


    formLayout->setWidget(0, QFormLayout::LabelRole, label);
    formLayout->setWidget(1, QFormLayout::LabelRole, label_0);
    formLayout->setWidget(2, QFormLayout::LabelRole, label_1);

    formLayout->setWidget(0, QFormLayout::FieldRole, printerLineEdit);
    formLayout->setWidget(1, QFormLayout::FieldRole, secLabelLineEdit);
    formLayout->setWidget(2, QFormLayout::FieldRole, usersList);
    //------------------------ bottom line
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    //------------------------------- Нижний блок - Кнопки --------------------

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding,
                                                    QSizePolicy::Minimum);

    saveButton       = new QPushButton(this);
    cancelButton     = new QPushButton(this);

    saveButton->setText(QObject::trUtf8("Сохранить"));
    cancelButton->setText(QObject::trUtf8("Отменить"));
    cancelButton->setDefault(true);


    horizontalLayout->addWidget (saveButton);
    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(cancelButton);
    //-------------------------------------- Компоновка -----------------------

    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(line);
    mainLayout->addLayout(horizontalLayout);

    this->setLayout(mainLayout);

    connect (saveButton,SIGNAL( clicked() ),
             this, SLOT(accept())
            );
    connect (cancelButton,SIGNAL(clicked()),
             this,SLOT(reject())
            );
}

void EditPrinterProperty::setVisiblePartModel(QStandardModel *model,QModelIndex *root)
{
    usersList->setModel(model);
    usersList->setRootIndex(root);;
}
