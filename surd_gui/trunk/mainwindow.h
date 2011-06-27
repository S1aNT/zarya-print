#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QtGui/QDialog>
#include <QtGui/QDesktopWidget>
#include "mytypes.h"
#include "qledindicator.h"
#include "engine.h"
#include "networkclient.h"
//#include "exttreeview.h"

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
    void checkAuthUser(const QString &login, const QString &mandat);
    void networkEnabled();
    void do_error(VPrn::AppErrorType eCode,QString e_msg);
    void editSelectItem();

    void checkDataFill();
    void sendDataToNet();
    void help();

private:        
    Engine *m_engine;
    NetWorkClient *m_netClient;
    QDataWidgetMapper *mapper;

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

    /*

*/
    QLedIndicator *led_auth;
    QLedIndicator *led_surd;   

    QFrame         *line;
    QErrorMessage  *myEMsgBox;

    QPoint calcDeskTopCenter(int width,int height);

    bool readConfig(const QString &app_dir);
    void connectAll();
};

#endif // MAINWINDOW_H

