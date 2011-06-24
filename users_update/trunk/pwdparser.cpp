#include <QDebug>
#include "pwdparser.h"
#include <QFile>
#include <QTextStream>

PwdParser::PwdParser(QObject *parent)
    :   QObject(parent)
{

    client = new NetClient();
    connect(this,   SIGNAL(foundUser(QString)),
            client, SLOT (addUser(QString))
            );
}
PwdParser::~PwdParser()
{
    if (client){
        client->deleteLater();
    }
}

int PwdParser::findAndSendUser(const QString &serverName, int serverPort,int startPid)
{
    if (client) {
        client->connectToHost(serverName,serverPort);
        if (client->waitForConnected(5000)){
            //qDebug("Connected!");
            this->getUsers4File("/etc/passwd",startPid);
            return 0;
        }
    }
    return -1;
}

void PwdParser::getUsers4File(const QString &pfile,int start_uid)
{
    QFile f_in(pfile);

    if (f_in.exists(pfile)){

        if (f_in.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream s(&f_in);

            QString line;
            do {
                line = s.readLine();
                //qDebug() << "read line: " << line;
                QString u_name = line.section(":",0,0);
                int u_id = line.section(":",2,2).toInt();
                //qDebug() << "user: " << u_name << " uid: " << u_id;
                if (u_id >= start_uid){
                    emit foundUser(u_name);
                    //qDebug() << "found user: " << u_name;
                }
            }while (!s.atEnd());
            f_in.close();
        }

    }else{
        emit sendEventMessageInfo(
                    QObject::trUtf8("\nФайл [%1] не найден или отсутсвуют права доступа.")
                    .arg(pfile),
                    VPrn::eId_FileNotFound,
                    VPrn::Error );
    }
}
