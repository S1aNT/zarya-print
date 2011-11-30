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
class QStandardItemModel;
class QModelIndex;
class QAction;
QT_END_NAMESPACE

class EditPrinterProperty : public QDialog
{
    Q_OBJECT
public:
    explicit EditPrinterProperty(QWidget *parent = 0);
    void setVisiblePartModel(QStandardItemModel *model,QModelIndex &root);
    void setPrinterAtrib(const QString &name, const QString &mandat);
signals:

public slots:
private slots:
    void addUser();
    void delUser();
    void editUser();
    void showContextMenu(const QPoint &pnt);
private:
    QLineEdit *printerLineEdit;
    QTreeView *usersList;
    QLineEdit *secLabelLineEdit;


    QPushButton *saveButton;
    QPushButton *cancelButton;
    QAction *m_addAction;
    QAction *m_delAction;
    QAction *m_editAction;
};

#endif // EDITPRINTERPROPERTY_H
