#include <QtCore/QCoreApplication>
#include "pwdparser.h"
#include "netmsgsender.h"
#include "message.h"
#include <QTimer>
#include <iostream>
#include "config.h"
#include <QTextCodec>

void print_help()
{
    std::cout << "Usage: users_update [--help] [--server=<IP or Hostname>] [--port=<Port>] --start_pid=<PID> " << std::endl;
    std::cout << std::endl;
    std::cout << "Example: users_update --server=192.168.112.2 --port=4243 --start_pid=499" << std::endl;
    std::cout << "OR shot  users_update -s=192.168.112.2 -p=4243 -sp=499" << std::endl;

}

void readConfig(const QString &ini_path,QString &sName,qint16 &sPort,qint16 &pid)
{

}

void ParseCmdLine(const QStringList &arg,QString &sName,qint16 &sPort,qint16 &pid)
{
    foreach( QString ar, arg) {
        if ( ar.split('=').size() == 2 ) {
            QString key   = ar.split('=').at(0);
            QString value = ar.split('=').at(1);

            if ((key.compare("--server",Qt::CaseInsensitive)==0)||
                    (key.compare("-s",Qt::CaseInsensitive)==0) ){
                sName = value;
            }
            if ((key.compare("--port",Qt::CaseInsensitive)==0) ||
                    (key.compare("-p",Qt::CaseInsensitive)==0) ) {
                sPort = value.toInt();

            }
            if ((key.compare("--start_pid",Qt::CaseInsensitive)==0 ) ||
                    (key.compare("-sp",Qt::CaseInsensitive)==0 ) ){
                pid = value.toInt();
            }
        }
        else {
            if ( ar == "--help" ){
                ;
            }
        }
    }
    return;
}

int main(int argc, char *argv[])
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
#if defined(Q_WS_WIN)
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("CP866"));
#else
    QTextCodec::setCodecForLocale(codec);
#endif
    QCoreApplication app(argc, argv);
    // installLog("user_updates",QObject::trUtf8("Zarya"));
    std::cout << "Reader users from /etc/passwd and sending ControlServer." << std::endl;
    std::cout << "Created by Sl@NT <p.slant@gmail.com>" << std::endl;
    std::cout << std::endl;

    QString serverName=QString();
    qint16 serverPort=0;
    qint16 startPid=0;

    ParseCmdLine(app.arguments(),serverName,serverPort,startPid);
    if ( app.arguments().isEmpty() ){
        readConfig( app.applicationDirPath(),serverName,serverPort,startPid );
    }

    if (serverName.isEmpty() || serverPort ==0 || startPid == 0)  {
        print_help();
        return -1;
    }
    // start killer for app
    //QTimer::singleShot(15000, &app, SLOT(quit()));

    NetMsgSender netMsgSender(0);
    PwdParser pwdParser;

    QObject::connect( &netMsgSender, SIGNAL(connected()),
                     &pwdParser,    SLOT (startFindUsers())
                     );
    QObject::connect( &pwdParser,     SIGNAL(foundUsers(QStringList &)),
                     &netMsgSender,  SLOT  (sendUsersToServer(QStringList &))
                     );
    QObject::connect( &netMsgSender, SIGNAL( finished() ), &app, SLOT( quit() ) );


    pwdParser.setStartPid(startPid);
    netMsgSender.setServer(serverName,serverPort);

    return app.exec();
}
