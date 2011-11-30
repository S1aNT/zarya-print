#ifndef SELECTWAY_H
#define SELECTWAY_H

#include <QtGui/QDialog>
#include <QCloseEvent>
#include <QSignalMapper>

QT_FORWARD_DECLARE_CLASS ( QPushButton )

class SelectWay : public QDialog
{
    Q_OBJECT
public:    
    SelectWay(QWidget *parent = 0);
signals:
    void selPrint();
    void selPreView();
    void selPartPreView();

private slots:
    void selectButton(int btn);

private:
    QSignalMapper *signalMapper;

    QPushButton *printButton;
    QPushButton *previewAllButton;
    QPushButton *previewPartButton;
    QPushButton *contEditButton;
};

#endif // SELECTWAY_H
