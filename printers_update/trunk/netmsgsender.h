#ifndef NETMSGSENDER_H
#define NETMSGSENDER_H

#include <QObject>
#include "netclient.h"
#include "message.h"

using namespace VPrn;

/**
  * @class NetMsgSender
  * @brief  Данный класс служит для отправки сообщений формата @sa Message
  * по сети управляющему серверу (УС). Является ядром консольного приложения
  * При создании пытается установить связь с УС и при успехе отправляет сигнал
  * connected
  * При вызове метода @fn finish() отправляет сигнал finished(), что приводит
  * к завершению работы консольного приложения
  */
class NetMsgSender : public QObject
{
    Q_OBJECT
public:
    explicit NetMsgSender(QObject *parent = 0);
    ~NetMsgSender();
    void setServer(const QString &host=QString(),qint16 port=0);
signals:
    void connected();
    void finished();
    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);
public slots:
    void finish();
    void sendPrintersToServer(QStringList &prn_list);
    void sendMsgToServer(const Message &msg);

private slots:

   void parseMessage(const Message &msg);
private:
      NetClient *client;
      PACKET_SIZE packetSize;

};

#endif // NETMSGSENDER_H
