#include "networkclient.h"
#include <QDataStream>
#include <QByteArray>
#include <QTextStream>

NetWorkClient::NetWorkClient(QObject *parent)
    : QTcpSocket(parent)
    , packetSize(-1)
{

    connect(this, SIGNAL(connected()),
            this, SIGNAL(connectToRemoteServer())
            );

    connect(this, SIGNAL(readyRead()),
            this, SLOT(onReadyRead())
            );
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError))
            );

}

void NetWorkClient::reSendNetworkMessage(const Message &msg)
{
    if (this->state() == QAbstractSocket::ConnectedState){
        this->write( msg.createPacket() );
    }
}
//------------------------- private slots

void NetWorkClient::onReadyRead()
{
    QDataStream in ( this );
    in.setVersion(QDataStream::Qt_4_0); // Для совместимости с серверной частью

    while (this->bytesAvailable() > 0){
        if (packetSize == -1) {
            //Определим количество байт доступных для чтения min >= 8 byte
            if( (PACKET_SIZE) this->bytesAvailable() < (PACKET_SIZE) sizeof(packetSize) ){
                return;
            }            
            in >> packetSize;//Читаем размер пакета
        }
        //Проверим что в сокет пришел весь пакет а не его огрызки
        if (this->bytesAvailable() < packetSize){
            return;
        }
        //Сбросим размер пакета, для обработки следующего
        packetSize = -1;
        // Прочтем тип сообщения
        int m_Type;
        in >> m_Type;

        //Прочтем само сообщение
        QByteArray data;
        in >> data;
        Message message( this );
        message.setType((MessageType) m_Type);
        message.setMessageData( data );
        // Отправка сообщения
        emit reciveMessage( message );
    }
}

void NetWorkClient::onError(QAbstractSocket::SocketError eCode)
{
    QString e_msg;
    switch(eCode)
        {
    case QAbstractSocket::ConnectionRefusedError :
            e_msg =QObject::trUtf8("Соединение отклоненно удаленным сервером [%1]")
                    .arg(this->peerName() );
            break;
        case QAbstractSocket::HostNotFoundError :
            e_msg =QObject::trUtf8("Удаленный сервер [%1] не найден!")
                    .arg(this->peerName());
            break;
        case QAbstractSocket::SocketTimeoutError :
            e_msg =QObject::trUtf8("Превышено время ожидания ответа от сервера [%1]")
                    .arg(this->peerName());
            break;
        case QAbstractSocket::NetworkError:
            e_msg =QObject::trUtf8("Общая ошибка сети.Возможно не подключен сетевой кабель..");
            break;
        default:
            e_msg = QObject::trUtf8("Код ошибки %1").arg(eCode,0,10);
        }
    e_msg.append("\n").append(QString(Q_FUNC_INFO));

    emit sendEventMessageInfo(e_msg,VPrn::eId_NetworkError,VPrn::Error,
                              VPrn::eCatId_DebugInfo
                              );
}
