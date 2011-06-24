#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QtGui/QDialog>
#include <QtGui/QDesktopWidget>
#include "mytypes.h"
#include "qledindicator.h"
#include "engine.h"
#include "networkclient.h"

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
    void checkAuthUser(const QString &login, const QString &mandat);
    void do_needAuthUser(const QString &login_mandat_list);
    void do_error(VPrn::AppErrorType eCode,QString e_msg);
    void parseNetworkMessage(const QString & msg);
    void networkEnabled();
    void checkDataFill();
    void sendDataToNet();

private:        
    Engine *m_engine;
    NetWorkClient *m_netClient;

    QString serverName;
    qint32 serverPort;


    QDesktopWidget desktop;
    QRect desktop_avail;

    QHBoxLayout    *horizontalLayout;
    QVBoxLayout    *mainLayout;

    QPushButton    *helpButton;    
    QPushButton    *sendButton;
    QPushButton    *exitButton;
    QSpacerItem    *horizontalSpacer;
    QSpacerItem    *horizontalSpacer_2;

    QLineEdit *LoginlineEdit;
    QLineEdit *MandatlineEdit;

    QLineEdit *NameZPlineEdit;
    QLineEdit *NameGMlineEdit;
    QLineEdit *DayMlineEdit;
    QLineEdit *VRNTlineEdit;
    QLineEdit *NBMlineEdit;
    QLineEdit *ReqlineEdit;
    QCheckBox *TracecheckBox;
    QLedIndicator *led_auth;
    QLedIndicator *led_surd;   

    QFrame         *line;
    QErrorMessage  *myEMsgBox;


    QPoint calcDeskTopCenter(int width,int height);

    bool readConfig(const QString &app_dir);

};

#endif // MAINWINDOW_H

