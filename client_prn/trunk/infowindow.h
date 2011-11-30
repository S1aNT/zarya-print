#ifndef INFOWINDOW_H
#define INFOWINDOW_H

#include <QDialog>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

class InfoWindow : public QDialog
{
    Q_OBJECT
public:
    explicit InfoWindow(QWidget *parent = 0);    
    void setText (const QString &text);
signals:

public slots:

private:
    QPlainTextEdit *plainTextEdit;
    QPushButton *pushButton;
};

#endif // INFOWINDOW_H
