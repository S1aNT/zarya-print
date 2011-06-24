#ifndef MSGDISPATCHER_H
#define MSGDISPATCHER_H

#include <QObject>
#include "message.h"
#include "mytypes.h"

using namespace VPrn;

class MsgDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit MsgDispatcher(QObject *parent = 0);

signals:
    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);

public slots:
    void parseMessage(const QString &c_uuid,const Message &msg);
};

#endif // MSGDISPATCHER_H
