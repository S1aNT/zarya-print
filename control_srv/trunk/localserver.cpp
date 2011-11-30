#include "localserver.h"
#include <QtCore/QUuid>
#include <QMapIterator>
#include <QByteArray>

LocalServer::LocalServer(QObject* parent)
        : QTcpServer(parent)
        , disabled(true)
        , packetSize(-1)
{
    connect(this, SIGNAL(newConnection()),
            this, SLOT(client_init()));
}


void LocalServer::pause()
{
    disabled = true;
    this->setMaxPendingConnections(0);
}

void LocalServer::resume()
{
    disabled = false;
    this->setMaxPendingConnections(30);    
}

void LocalServer::sendMessage (const QString &c_uuid,const Message &msg)
{
    QTcpSocket *client(0);
    // По UUID определим через какого клиента надо отправить это сообщение
    client = findClient(c_uuid);
    if (client){
        qDebug() << "Send message to: " <<c_uuid;
        //Сформируем пакет И пошлем его ветром гонимого клиенту
        client->write(msg.createPacket());
        //client->flush();
    }
}
//------------------------------ private slots----------------------------------
void LocalServer::client_init()
{
    if ( disabled){
        return;
    }

    QTcpSocket *client = this->nextPendingConnection();
    if (client){
        QString c_uuid = getUuid();
        clients.insert(client);
        clients_uuid.insert(client,c_uuid);

        connect(client, SIGNAL(readyRead()),
                this,   SLOT(readyRead()));
        connect(client, SIGNAL(disconnected()),
                this,   SLOT(disconnected()));
        connect(client, SIGNAL(error(QAbstractSocket::SocketError)),
                this,   SLOT(prepareError(QAbstractSocket::SocketError)));

        emit sendEventMessageInfo(QObject::trUtf8("Новый клиент подключен!"),
                                  VPrn::eId_DebugInfo,
                                  VPrn::Information,VPrn::eCatId_DebugInfo);
        //emit newClientConnected(c_uuid);
    }
}

void LocalServer::readyRead()
{

    QTcpSocket *client = qobject_cast<QTcpSocket*>( QObject::sender() );
    QString client_uuid = clients_uuid.value(client);

    if (client){
        emit sendEventMessageInfo(QObject::trUtf8("Пришло сообщение от клиента [%1]").arg(client_uuid),
                                  VPrn::eId_DebugInfo,
                                  VPrn::Information,VPrn::eCatId_DebugInfo);

        qDebug() << "Recive message from: " <<client_uuid;

        QDataStream in ( client );
        in.setVersion(QDataStream::Qt_4_0); // Для совместимости с серверной частью

        while (client->bytesAvailable() > 0){
            if (packetSize == -1) {
                //Определим количество байт доступных для чтения min >= 8 byte
                if( (PACKET_SIZE) client->bytesAvailable() <
                    (PACKET_SIZE) sizeof(packetSize) ){
                    return;
                }
                in >> packetSize;//Читаем размер пакета
            }
            //Проверим что в сокет пришел весь пакет а не его огрызки
            if (client->bytesAvailable() < packetSize){
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
            emit reciveMessage( client_uuid, message );
        }
    }
}

void LocalServer::disconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>( QObject::sender() );
    QString client_uuid = clients_uuid.value(client);

    clients.remove(client);
    clients_uuid.remove(client);
    emit sendEventMessageInfo(QObject::trUtf8("Клиент отключен!"),
                              VPrn::eId_DebugInfo,
                              VPrn::Information,VPrn::eCatId_DebugInfo);
    //Отправка сообщения всем что клиент с таким UUID умер
    //emit currentClientDisConnected(client_uuid);
}

void LocalServer::prepareError(QAbstractSocket::SocketError socketError)
{
    QString e_msg;
    QTcpSocket *client = qobject_cast<QTcpSocket*>( QObject::sender() );
    switch(socketError)
    {
    case QAbstractSocket::ConnectionRefusedError :
        e_msg =QObject::trUtf8("Соединение отклоненно удаленным клиентом [%1]")
               .arg(client->peerName() );
        break;
    case QAbstractSocket::RemoteHostClosedError:
        e_msg =QObject::trUtf8("Соединение закрыто удаленным клиентом ");

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

QTcpSocket *LocalServer::findClient(const QString &c_uuid)
{
    QTcpSocket *client(0);
    QMapIterator<QTcpSocket *,QString>  i(clients_uuid);
    while (i.hasNext()) {
        i.next();
        //qDebug() << "\nClient Key: " << i.key() << " value: " << i.value();
        if (i.value() == c_uuid ){
            return i.key(); //Я работаю только с живыми клиентами
        }
    }
    // Только некромантам интересны мертвые клиенты
    emit sendEventMessageInfo(
            QObject::trUtf8("Ответ клиенту который уже отсоединился!\n %1.")
            .arg(QString(Q_FUNC_INFO) ),
            VPrn::eId_NetworkError,
            VPrn::Error,
            VPrn::eCatId_DebugInfo );
    return client;
}

QString LocalServer::getUuid() const
{
    return QUuid::createUuid().toString().replace("{","").replace("}","");
}

/*
        emit sendEventMessageInfo(
                QObject::trUtf8("\nВнутрення ошибка приложения.Обратитесь к разработчику!."),
                VPrn::eId_AppsError,
                VPrn::Error );
                */


