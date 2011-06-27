#ifndef EDITPRINTERPROPERTY_H
#define EDITPRINTERPROPERTY_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLabel;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;
class QSpacerItem;
class QComboBox;
class QTreeView;
class QPushButton;
class QStandardModel;
class QModelIndex;
QT_END_NAMESPACE

class EditPrinterProperty : public QDialog
{
    Q_OBJECT
public:
    explicit EditPrinterProperty(QWidget *parent = 0);
    void setVisiblePartModel(QStandardModel *model,QModelIndex *root);
signals:

public slots:
private:
    QLineEdit *printerLineEdit;
    QTreeView *usersList;
    QLineEdit *secLabelLineEdit;


    QPushButton *saveButton;
    QPushButton *cancelButton;
};

#endif // EDITPRINTERPROPERTY_H
