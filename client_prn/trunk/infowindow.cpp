#include "infowindow.h"

InfoWindow::InfoWindow(QWidget *parent) :
    QDialog(parent)
{
    this->resize(540, 240);
    this->setMaximumSize(540,240);

    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    plainTextEdit = new QPlainTextEdit();
    plainTextEdit->setReadOnly(true);
    plainTextEdit->setCenterOnScroll(true);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QSpacerItem *verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();

    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    pushButton = new QPushButton(QObject::trUtf8("Закрыть"));
    QSpacerItem *horizontalSpacer_2 = new QSpacerItem(48, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(pushButton);
    horizontalLayout->addItem(horizontalSpacer_2);

    verticalLayout->addWidget(plainTextEdit);
    verticalLayout->addItem(verticalSpacer);
    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addItem(verticalSpacer_2);

    this->setLayout(verticalLayout);
};


void InfoWindow::setText(const QString &text)
{
   plainTextEdit->setPlainText(text);
}
