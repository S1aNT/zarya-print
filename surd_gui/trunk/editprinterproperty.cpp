#include "editprinterproperty.h"
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QRegExp>
#include <QTreeView>
#include <QLabel>
#include <QFont>
#include <QStandardItemModel>
#include <QAction>
#include <QInputDialog>

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
    usersList->setColumnHidden(2,true);
    usersList->setContextMenuPolicy(Qt::CustomContextMenu);

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

    // create an add action and connect it to a signal
    m_addAction  = new QAction(QObject::trUtf8("Добавить пользователя"),usersList );
    m_delAction  = new QAction(QObject::trUtf8("Удалить пользователя"),usersList );
    m_editAction = new QAction(QObject::trUtf8("Редактировать пользователя"),usersList );

    connect(m_addAction, SIGNAL(triggered()),
            this, SLOT(addUser())
            );
    connect(m_delAction, SIGNAL(triggered()),
            this, SLOT(delUser())
            );
    connect(m_editAction, SIGNAL(triggered()),
            this, SLOT(editUser())
            );

    // connect custom context menu
    connect(usersList, SIGNAL(customContextMenuRequested( const QPoint& )),
            this, SLOT(showContextMenu(const QPoint &))
            );

    connect (saveButton,SIGNAL( clicked() ),
             this, SLOT(accept())
             );
    connect (cancelButton,SIGNAL(clicked()),
             this,SLOT(reject())
             );
}

void EditPrinterProperty::setVisiblePartModel(QStandardItemModel *model,QModelIndex &root)
{
    usersList->setModel(model);
    usersList->setRootIndex(root);    
}

void EditPrinterProperty::setPrinterAtrib(const QString &name, const QString &mandat)
{
    printerLineEdit->setText(name);
    secLabelLineEdit->setText(mandat);
}

//------------------------- Private slots --------------------------------------
void EditPrinterProperty::delUser()
{
    QModelIndex idx = usersList->currentIndex();

}
void EditPrinterProperty::editUser()
{
    QModelIndex idx = usersList->currentIndex();

}

void EditPrinterProperty::addUser()
{
    QModelIndex idx = usersList->currentIndex();
    bool ok;
    QString text = QInputDialog::getText(this, tr("Введите логин и мандат пользователя в формате name,S1:C2"),
                                         tr("Добавить:"), QLineEdit::Normal,
                                         QObject::trUtf8("usr13,S12:C127"),
                                         &ok);
    if (ok && !text.isEmpty()) {

        QString login  = text.section(",",0,0);
        QString mandat = text.section(",",1,1);

        QModelIndex idx = usersList->rootIndex();
        int i_row = idx.row();
        usersList->model()->insertRow(0,idx);

        usersList->model()->setData(usersList->model()->index(0,0,idx),login,Qt::EditRole);
        usersList->model()->setData(usersList->model()->index(0,1,idx),mandat,Qt::EditRole);
    }
}

void EditPrinterProperty::showContextMenu(const QPoint &pnt)
{
    QList<QAction *> actions;
    if (usersList->indexAt(pnt).isValid()) {
        actions.append(m_addAction);
        actions.append(m_delAction);
        actions.append(m_editAction);
    }
    if (actions.count() > 0)
        QMenu::exec(actions, usersList->mapToGlobal(pnt));
}

