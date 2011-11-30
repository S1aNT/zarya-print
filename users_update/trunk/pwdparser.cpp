#include <QDebug>
#include "pwdparser.h"
#include <QFile>
#include <QTextStream>

PwdParser::PwdParser(QObject *parent)
        :   QObject(parent)
        , m_pid(0)
{


}



void  PwdParser::startFindUsers()
{
    qDebug() << Q_FUNC_INFO;
    getUsers4File("/etc/passwd",m_pid);
}

void PwdParser::getUsers4File(const QString &pfile,int start_uid)
{
    QStringList userList;
#if defined(Q_OS_WIN)

    QString u_name;
    for (int i=0;i<3;i++){
        u_name = QObject::tr("user_%1").arg(i,0,10);
        userList.append(u_name);

    }
    //Отправка списка
        emit foundUsers(userList);
#elif defined(Q_OS_UNIX)
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
                    userList.append(u_name);

                    //qDebug() << "found user: " << u_name;
                }
            }while (!s.atEnd());
            f_in.close();
        }
        //Отправка списка
        emit foundUsers(userList);
    }else{
        emit sendEventMessageInfo(
                QObject::trUtf8("\nФайл [%1] не найден или отсутсвуют права доступа.")
                .arg(pfile),
                VPrn::eId_FileNotFound,
                VPrn::Error );
    }
#endif
}
