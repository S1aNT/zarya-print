#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QtGui/QDialog>
#include <QtGui/QDesktopWidget>
#include "mytypes.h"
#include "qledindicator.h"
#include "engine.h"
#include "localdb.h"



QT_BEGIN_NAMESPACE
        class QAction;
class QStackedWidget;
class QFrame;
class QCheckBox;
class QLabel;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;
class QSpacerItem;
class QErrorMessage;
class QComboBox;
class QListWidget;
class QDataWidgetMapper;
class QTreeView;
class QModelIndex;
QT_END_NAMESPACE

        namespace Ui {
    class MainWindow;
}

class Engine;

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init(const QString &app_dir);

public slots:
    void appendStartMsg(const QString &txt_msg);


private slots:

    void netLedEnabled();
    void authLedEnabled();
    void do_error(VPrn::AppErrorType eCode,QString e_msg);
    void editSelectItem();

    void help();

/**
      * @fn void enableEditMode();
      * @param state Триггер вкл/выкл кнопки редактирования
      * @brief Режим редактирования становиться доступным для пользователя только после выполнения
      * условий 1. Есть связь с УС ,2. Пользователь авторизован на УС для этой работы
      */
    void enableEditMode();

private:        
    LocalDB *localdb;
    Engine  *engine;

    QString serverName;
    qint32 serverPort;

    QDesktopWidget desktop;
    QRect desktop_avail;

    QHBoxLayout    *horizontalLayout;
    QVBoxLayout    *mainLayout;

    QPushButton    *helpButton;    
    QPushButton    *editButton;
    QPushButton    *exitButton;
    QSpacerItem    *horizontalSpacer;
    QSpacerItem    *horizontalSpacer_2;

    QTreeView      *prnTreeView;

    QLedIndicator *led_auth;
    QLedIndicator *led_net;

    QFrame         *line;
    QErrorMessage  *myEMsgBox;

    QPoint calcDeskTopCenter(int width,int height);

    bool readConfig(const QString &app_dir);
    void connectAll();


};

#endif // MAINWINDOW_H

