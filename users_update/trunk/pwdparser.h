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
    ~PwdParser();

    int findAndSendUser(const QString &serverName, int serverPort,int startPid);

signals:
    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);
    void foundUser(const QString &u_name);
public slots:
private:
    NetClient *client;

    /**
      * @fn void getUsers4File(const QString &pfile,int start_uid);
      * @brif Читает файл /etc/passwd получает из него список
      * реальных пользователей для Красной Шапки  id >499
      */
    void getUsers4File(const QString &pfile,int start_uid);
};

#endif // PWDPARSER_H
