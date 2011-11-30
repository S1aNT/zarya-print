#include <QDebug>
#include <QLibrary>
#include <QSettings>
#include <QProcess>
#include <QString>


#include "auth_plugin.h"

AuthPlugin::AuthPlugin(QObject*parent)
    :QObject(parent)
{

}

void AuthPlugin::init()
{
#ifdef DEBUG
    user = "usr_1";
    role  = "user_r";
    mlsRange   = "s0";
    mcsRange   = "c0.c512";
    emit userAuth(user,role,mlsRange,mcsRange);
#else

#if defined(Q_OS_LINUX)
    QProcess proc;
    QString m_Output;

    proc.setProcessChannelMode( QProcess::SeparateChannels );
    proc.start( "id - Z" );
    if (!proc.waitForStarted(1000)) {
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

        emit sendEventMessageInfo(QObject::trUtf8("Ошибка %1\nПри запуске приложения id -Z !\n%2")
                                  .arg(e_info)
                                  .arg(QString(Q_FUNC_INFO)),
                                        VPrn::EventLogMessageId eventCode,
                                        VPrn::EventLogType      evenType,
                                        VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);

        qDebug()<< m_Output << " QProcess::error() " << proc.error();
    }else{
        proc.waitForFinished(-1);
        proc.closeWriteChannel();
        m_Output = proc.readAll();//.trimmed();
        // логон:роль:домен:mls:msc;

        // unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023
        qDebug() << Q_FUNC_INFO << "m_Output " << m_Output << "\n";
        QStringList context = m_Output.split(":");
        if (context.count == 4){
            user = context.at(0); // логон
            role  = context.at(1);
            mlsRange   = context.at(3);
            mcsRange   = context.at(4);
            emit userAuth(user,role,mlsRange,mcsRange);
        }
    }

#endif

#endif
}

Q_EXPORT_PLUGIN2(auth_plugin, AuthPlugin);

