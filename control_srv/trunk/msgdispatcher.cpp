#include "msgdispatcher.h"

MsgDispatcher::MsgDispatcher(QObject *parent) :
    QObject(parent)
{
}


//------------------- Public slots ---------------------------------------------
void MsgDispatcher::parseMessage(const QString &c_uuid,const Message &msg)
{

}

//------------------------------------------------------------------------------
