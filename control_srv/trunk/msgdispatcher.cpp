#include "msgdispatcher.h"
#include <QStringList>
#include <QDataStream>
#include <QByteArray>


MsgDispatcher::MsgDispatcher(QObject *parent) :
        QObject(parent)
{
}


//------------------- Public slots ---------------------------------------------
void MsgDispatcher::parseMessage(const QString &c_uuid,const Message &msg)
{
    //qDebug() << "recive msg from " << c_uuid;
    QString str_cmd;
    QStringList m_List;

    MessageType mType = msg.getType();
    switch (mType){
    case VPrn::Que_GetDataBaseSlice:
        {
            QByteArray m_data = msg.getMessageData();
            QString login;
            QString mandat;
            QDataStream in(&m_data, QIODevice::ReadOnly );
            in.setVersion(QDataStream::Qt_4_0);
            in >> login;
            in >> mandat;
            emit getDataBaseSlice(c_uuid,login,mandat);
        }
        break;
    case VPrn::Cmd_SaveUserToBase:
        m_List = msg.getMessageDataList();
        emit saveUserToBase (c_uuid, m_List );
        break;
    default:
        break;
    }

}

void MsgDispatcher::userListSaved(const QString &c_uuid,bool state)
{
    Message msg;

    msg.setType(VPrn::Ans_userListSavedToBase);
    if (state){
        msg.setMessageData(QObject::trUtf8("Запись списка пользователей в базу завершена успешно.").toUtf8());
    }else{
        msg.setMessageData(QObject::trUtf8("Запись списка пользователей в базу завершена с ошибкой.").toUtf8());
    }
    emit sendMsgToClient(c_uuid,msg);
}

void MsgDispatcher::setDataBaseSlice( const QString &c_uuid,const QByteArray &data)
{
    Message msg;
    msg.setType(VPrn::Ans_GetDataBaseSlice);
    msg.setMessageData(data);
    emit sendMsgToClient(c_uuid,msg);
}
//------------------------------------------------------------------------------
