#ifndef MSGDISPATCHER_H
#define MSGDISPATCHER_H

#include <QObject>
#include "message.h"
#include "mytypes.h"

class QByteArray;

using namespace VPrn;

class MsgDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit MsgDispatcher(QObject *parent = 0);

signals:
    /** @brief
      * Запрос к БД на получение списка принтеров, пользователей и расширенных атрибутов
      * в соответствии с мандатом пользователя
      */
    void getDataBaseSlice(const QString &c_uuid,const QString &login,const QString &mandat);
    void sendMsgToClient(const QString &c_uuid,const Message &msg);
    void saveUserToBase(const QString &c_uuid,QStringList &u_list);
    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);

public slots:
    void setDataBaseSlice( const QString &c_uuid,const QByteArray & data);

    void parseMessage(const QString &c_uuid,const Message &msg);
    void userListSaved(const QString &c_uuid,bool state);
};

#endif // MSGDISPATCHER_H
