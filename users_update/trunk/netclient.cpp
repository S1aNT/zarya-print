#include "netclient.h"
#include <QAbstractSocket>
#include <QByteArray>


NetClient::NetClient(QTcpSocket *parent)
    :QTcpSocket (parent)
    , packetSize(-1)
{
    connect(this, SIGNAL(connected()),
            this,   SLOT(connected()));
    connect(this, SIGNAL(disconnected()),
            this,   SLOT(disconnected()));


    connect(this, SIGNAL(readyRead()),
            this,   SLOT(readyRead()));

    connect(this, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this,   SLOT(prepareError(QLocalSocket::LocalSocketError)));


}

void NetClient::addUser(const QString &user_name)
{
    if (!user_name.isEmpty()){
        Message message( this );
        message.setType(VPrn::SaveUserToBase);
        message.setMessageData( user_name.toUtf8() );
        this->write(message.createPacket());
        //qDebug() << Q_FUNC_INFO << " Write to socket :" << user_name;
    }
}
//------------------- Private slots -------------------------------------------
void NetClient::connected()
{
    //qDebug() << Q_FUNC_INFO << " Connected";
    emit sendEventMessageInfo(QObject::trUtf8("Установленно соединени с сервером!"),
                              VPrn::eId_DebugInfo,
                              VPrn::Information,VPrn::eCatId_DebugInfo);
}

void NetClient::readyRead()
{
    QDataStream in ( this );
    in.setVersion(QDataStream::Qt_3_0); // Для совместимости с серверной частью

    while (this->bytesAvailable() > 0){
        if (packetSize == -1) {
            //Определим количество байт доступных для чтения min >= 8 byte
            if( (PACKET_SIZE) this->bytesAvailable() <
                    (PACKET_SIZE) sizeof(packetSize) ){
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

void NetClient::disconnected()
{
    emit sendEventMessageInfo(QObject::trUtf8("Сетевое соединение закрыто!"),
                              VPrn::eId_DebugInfo,
                              VPrn::Information,VPrn::eCatId_DebugInfo);
    //Отправка сообщения всем что клиент с таким UUID умер
    //emit currentClientDisConnected(client_uuid);
}

void NetClient::prepareError(QAbstractSocket::SocketError socketError)
{
    QString e_msg;
    QTcpSocket *client = qobject_cast<QTcpSocket*>( QObject::sender() );
    switch(socketError)
        {
    case QAbstractSocket::ConnectionRefusedError :
            e_msg =QObject::trUtf8("Соединение отклоненно удаленным сервером [%1]")
                    .arg(client->peerName() );
            break;
        case QAbstractSocket::HostNotFoundError :
            e_msg =QObject::trUtf8("Удаленный сервер [%1] не найден!")
                    .arg(client->peerName());
            break;
        case QAbstractSocket::SocketTimeoutError :
            e_msg =QObject::trUtf8("Превышено время ожидания ответа от клиента [%1]")
                    .arg(client->peerName());
            break;
        case QAbstractSocket::NetworkError:
            e_msg =QObject::trUtf8("Общая ошибка сети.Возможно не подключен сетевой кабель..");
            break;
        default:
            e_msg = QObject::trUtf8("Код ошибки %1").arg(socketError,0,10);
        }
    e_msg.append("\n").append(QString(Q_FUNC_INFO));

    emit sendEventMessageInfo(e_msg,VPrn::eId_NetworkError,VPrn::Error,
                              VPrn::eCatId_DebugInfo
                              );
}
