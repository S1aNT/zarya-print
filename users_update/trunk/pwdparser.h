#ifndef PWDPARSER_H
#define PWDPARSER_H

#include <QObject>

#include "mytypes.h"
#include "netclient.h"

using namespace VPrn;

class PwdParser : public QObject
{
    Q_OBJECT
public:
    explicit PwdParser(QObject *parent = 0);

    void setStartPid(qint16 sPid){m_pid = sPid;}


signals:
    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);
    void foundUsers(QStringList &u_list);
public slots:
    void startFindUsers();

private:
    qint16 m_pid;

    /**
      * @fn void getUsers4File(const QString &pfile,int start_uid);
      * @brif Читает файл /etc/passwd получает из него список
      * реальных пользователей для Красной Шапки  id >499
      */
    void getUsers4File(const QString &pfile,int start_uid);
};

#endif // PWDPARSER_H
