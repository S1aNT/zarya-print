#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H
/**
  * @class NetWorkClient
  * @brief Класс предназанченный для приема и передачи сообщений,
  * @fn onReadyRead() склеивает пакеты пришедшие из сети в одно сообщение
  * @sa @class Message и выдает сигнал @fn reciveMessage(const Message &r_msg)
  * содержащий сообщение
  */

#include <QTcpSocket>
#include <QStack>
#include "mytypes.h"
#include "message.h"

using namespace VPrn;

class NetWorkClient : public QTcpSocket
{
    Q_OBJECT

public:
    NetWorkClient(QObject *parent=0);

signals:
    /**
      * @fn void reciveMessage(const Message &msg);
      * @brief Обработка сообщения полученного из сети
      */
    void reciveMessage(const Message &r_msg);
    void connectToRemoteServer();

    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory);

public slots:
    void reSendNetworkMessage(const Message &msg);

private slots:

    void onReadyRead();   
    void onError(QAbstractSocket::SocketError eCode);
private:
    /**
      * @var packetSize; Размер передаваемого блока данных
      */
    PACKET_SIZE packetSize;


};

#endif // NETWORKCLIENT_H
