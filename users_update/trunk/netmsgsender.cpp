#include "netmsgsender.h"
#include <QDataStream>
#include <QDebug>

NetMsgSender::NetMsgSender(QObject *parent)
    : QObject(parent)
    , client(0)
    , packetSize(-1)

{
    client = new NetClient();


    connect (client,SIGNAL(connected()),
             this,SIGNAL(connected())
             );

    connect(client, SIGNAL(reciveMessage(const Message &)),
            this,   SLOT(parseMessage(const Message &))
            );


}

NetMsgSender::~NetMsgSender()
{
    if (client){
        client->close();
        client->deleteLater();
    }
}

void NetMsgSender::setServer(const QString &host,qint16 port)
{
    if (client){
        client->connectToHost(host,port);
    }
}

//----------------------------- PUBLIC SLOTS -----------------------------------

void NetMsgSender::finish()
{
    if (client){
        client->close();
        client->deleteLater();
    }
    emit finished();
}

void NetMsgSender::sendUsersToServer(QStringList &usr_list)
{

    if (!usr_list.isEmpty() && client){
        Message message( this );
        message.setType(VPrn::Cmd_SaveUserToBase);
        message.setMessageData(usr_list);
        client->sendMessage( message);
        //qDebug() << Q_FUNC_INFO << " Write to socket :" << user_name;
    }

}

void NetMsgSender::sendMsgToServer(const Message &msg)
{
    if (client){
        client->sendMessage( msg);
    }
}

//------------------------ Private slots ---------------------------------------

void NetMsgSender::parseMessage(const Message &msg)
{
    emit sendEventMessageInfo(QObject::trUtf8("Пришло сообщение от сервера"),
                              VPrn::eId_DebugInfo,
                              VPrn::Information,VPrn::eCatId_DebugInfo);

    QString info;
    switch (msg.getType()){
    case VPrn::Cmd_FinishWork:

        emit finished();
        break;
    case VPrn::Ans_userListSavedToBase:
        info.append(msg.getMessageData());
        qDebug() << "Ans_userListSavedToBase:" << info;
        emit finished();
        break;
    default:
        break;
    }
}
