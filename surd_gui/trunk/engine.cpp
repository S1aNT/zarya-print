#include "engine.h"
#include <QSettings>
#include <QLibrary>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QList>

#include <QProcess>
#include <QDataStream>



#include "message.h"

Engine::Engine(QObject *parent)
        : QObject(parent)
        , m_netClient(0)

{

    this->getUserId();

    //Запуск сетевого клиента
    m_netClient = new NetWorkClient(this);


    //----------------------------- Netork client
    connect (m_netClient, SIGNAL( reciveMessage(const Message &)),
             this   , SLOT  ( parseNetworkMessage(const Message &))
             );

    connect (m_netClient, SIGNAL ( connectToRemoteServer()),
             this       , SIGNAL ( connected())
             );

    connect (m_netClient, SIGNAL( connectToRemoteServer()),
             this       , SLOT  ( authUser() )
             );

}

Engine::~Engine()
{


}

void Engine::setServer(const QString &host,qint16 port)
{
    if (m_netClient && !host.isEmpty() &&port !=0){
        m_netClient->connectToHost(host,port);
    }
}

//----------------- Public Slots -----------------------------------------------
void Engine::parseNetworkMessage(const Message & msg)
{
    MessageType mType = msg.getType();
    QByteArray m_data = msg.getMessageData();

    switch (mType){
    case VPrn::Ans_GetDataBaseSlice:
        emit saveBaseSlice(&m_data);
        emit authUserPass();
        break;
    default:
        break;
    }

}


void Engine::getUserId()
{
    QString m_Output;
#if defined(Q_WS_WIN)
    m_login = "usr_1";
    m_mandat= "S3:C245";

    emit definedAuthData(m_login,m_mandat);
#endif

#if defined(Q_OS_LINUX)
    QProcess proc;
    proc.setProcessChannelMode( QProcess::SeparateChannels );
    proc.start( "id - Z" );
    if (!proc.waitForStarted(5000)) {
        QString e_info;
        switch(proc.error()){
        case QProcess::FailedToStart:
            e_info = QObject::trUtf8("[Процесс не запущен или отстутсвуют права доступа на исполнение]");
            break;
        case QProcess::Crashed:
            e_info = QObject::trUtf8("[Процесс завершен с ошибкой, после успешного запуска]");
            break;
        case QProcess::Timedout:
            e_info = QObject::trUtf8("[Превышен предел ожидания ответа от процесса]");
            break;
        case QProcess::WriteError:
            e_info = QObject::trUtf8("[Ошибка записи в процесс (Процесс не запущен?)]");
            break;
        case QProcess::ReadError:
            e_info = QObject::trUtf8("[Ошибка чтения из процесса (Процесс не запущен?)]");
            break;
        case QProcess::UnknownError:
            e_info = QObject::trUtf8("[Неизвестная ошибка]");
            break;
        }
        emit error(VPrn::InternalAppError,
                   QObject::trUtf8("Ошибка %1\nПри запуске приложения id -Z !\n%2")
                   .arg(e_info)
                   .arg(QString(Q_FUNC_INFO))
                   );
        qDebug()<< m_Output << " QProcess::error() " << proc.error();
    }else{
        proc.waitForFinished(-1);
        proc.closeWriteChannel();
        m_Output = proc.readAll();//.trimmed();
        qDebug() << Q_FUNC_INFO << "m_Output " << m_Output << "\n";
        /// @todo Написать разборщик
        //Разберем ответ в формате
        m_login = "usr1";
        m_mandat= "S11:C12";

        emit definedAuthData(m_login,m_mandat);
    }

#endif
}



//------------------ Private Slots ----------------------------------------------------------
void Engine::authUser()
{

    if (!m_login.isEmpty() && !m_mandat.isEmpty() ){
        QByteArray m_data;
        QDataStream out(&m_data, QIODevice::WriteOnly );
        out.setVersion(QDataStream::Qt_4_0);
        out << m_login;
        out << m_mandat;

        Message msg;
        msg.setType(VPrn::Que_GetDataBaseSlice);
        msg.setMessageData(m_data);
        m_netClient->reSendNetworkMessage(msg);
    }
}

//-------------------- Private --------------------------------------------------------------
