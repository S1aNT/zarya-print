#ifndef EDITCARDPAGE_H
#define EDITCARDPAGE_H

#include <QWidget>
#include "mytypes.h"
using namespace VPrn;


QT_FORWARD_DECLARE_CLASS ( QDataWidgetMapper);
QT_FORWARD_DECLARE_CLASS ( QStandardItemModel )
QT_FORWARD_DECLARE_CLASS ( QStringListModel )
QT_FORWARD_DECLARE_CLASS ( QRadioButton )

QT_FORWARD_DECLARE_CLASS ( QByteArray )
QT_FORWARD_DECLARE_CLASS ( QCheckBox )
QT_FORWARD_DECLARE_CLASS ( QComboBox )
QT_FORWARD_DECLARE_CLASS ( QGroupBox )
QT_FORWARD_DECLARE_CLASS ( QLineEdit )
QT_FORWARD_DECLARE_CLASS ( QDateEdit )
QT_FORWARD_DECLARE_CLASS ( QSpinBox )
QT_FORWARD_DECLARE_CLASS ( QLabel )
QT_FORWARD_DECLARE_CLASS ( QStandardItemModel)
QT_FORWARD_DECLARE_CLASS ( QStringListModel )

class EditCardPage : public QWidget
{
    Q_OBJECT
public:
    EditCardPage(QWidget *parent = 0);
    void setPrinterModel(QStandardItemModel *pModel);
    void setCardModel(QStandardItemModel *cModel);
    void setStampModel(QStringListModel *sModel);
    void setTemplatesModel(QStandardItemModel *tModel);
    bool isCardValid(){return cardValid;}
signals:
    void enableNext();

public slots:
    void setCardValid(bool flag);

private slots:
    void checkNext();
private:
    bool cardValid;

    QDataWidgetMapper *mapper;

    QComboBox *secretCBox;
    QComboBox *templatesCBox;
    QComboBox *printersCBox;

    QLineEdit *punktLineEd;
    QLineEdit *mbNumberLineEd;
    QLineEdit *pagesCountLineEd;


    QLineEdit *executor_lineEd;
    QLineEdit *docName_lineEd;
    QLineEdit *pressman_lineEd;
    QLineEdit *invNumber_lineEd;
    QLineEdit *telephone_lineEd;
    QDateEdit *dateField_dateEd;

    QLineEdit *reciver_lineEd_1;
    QLineEdit *reciver_lineEd_2;
    QLineEdit *reciver_lineEd_3;
    QLineEdit *reciver_lineEd_4;
    QLineEdit *reciver_lineEd_5;

    QCheckBox *copy_checkBox_1;
    QCheckBox *copy_checkBox_2;
    QCheckBox *copy_checkBox_3;
    QCheckBox *copy_checkBox_4;
    QCheckBox *copy_checkBox_5;


    void connect_all();
};

#endif // EDITCARDPAGE_H
