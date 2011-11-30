#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QTcpSocket>
#include "message.h"
#include "mytypes.h"

using namespace VPrn;
class NetClient : public QTcpSocket
{
    Q_OBJECT
public:
    NetClient(QTcpSocket *parent=0);
signals:

    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);

    void reciveMessage(const Message &msg);
public slots:        
    void sendMessage(const Message &msg);

private slots:

    void connected();
    void readyRead();
    void disconnected();
    void prepareError(QAbstractSocket::SocketError socketError);

private:
    /**
      * @var packetSize; Размер передаваемого блока данных
      */
    PACKET_SIZE packetSize;
};

#endif // NETCLIENT_H
