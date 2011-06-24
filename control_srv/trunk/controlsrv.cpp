#include "controlsrv.h"
#include <QSettings>
#include <QFile>
#include <QByteArray>
#include <QDataStream>

ControlSrv::ControlSrv(int argc, char **argv)
    : QtService<QCoreApplication>(argc, argv, "CAPS_locServer")
    , eLogger(0)
    , locServer(0)
    , msgDispatcher(0)
    //, db_gateway(0)
    , Port(0)
{    
    setServiceDescription("A Fast TCP/IP service need's to Virtual Safe Printer.");
    setServiceFlags(QtServiceBase::CanBeSuspended);  
    // Создаем зависимые объекты
    eLogger = new ErrorLogger();    
    locServer = new LocalServer();
    msgDispatcher = new MsgDispatcher();

    // Все требуемые компонеты загружены, установим связи
    QObject::connect (locServer, SIGNAL(sendEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory)),
                      eLogger,   SLOT  (saveEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory))
                      );
    QObject::connect (msgDispatcher, SIGNAL(sendEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory)),
                      eLogger,       SLOT  (saveEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory))
                      );

    QObject::connect (locServer,     SIGNAL( reciveMessage(const QString &,const Message&) ),
                      msgDispatcher, SLOT  ( parseMessage(const QString &,const Message&)  )
                      );

}

// -------------------------- protected ----------------------------------------

void ControlSrv::start()
{
    QCoreApplication *app = application();

    // Чтение настроек приложения
    if ( eLogger && readConfig() && Port != 0  ){

        if (locServer->listen(QHostAddress::Any,Port) ) {
            locServer->resume();
            eLogger->saveEventMessageInfo(
                        QObject::trUtf8("\nЯдро поддержки виртуального защищенного принтера запущенно."),
                        eId_ServiceStarted,
                        VPrn::Information
                        );

        }else{
            eLogger->saveEventMessageInfo(
                        QObject::trUtf8("\nПорт %1 уже занят другим приложением!")
                        .arg(locServer->serverPort()),
                        eId_CanNotBindPort, VPrn::Error  );
            app->exit();
        }
    }else{
        eLogger->saveEventMessageInfo(
                    QObject::trUtf8("\nОшибка чтения ini файла!"),
                    VPrn::eId_CanNotReadConfig, VPrn::Error );
        app->quit();
    }

    /*
    // Создание  диспетчера сообщений

    db_gateway= new DB_GateWay();
    db_gateway->saveTemplatesInfoToBase( tFolders );

        eLogger->saveEventMessageInfo(
                QObject::trUtf8("\nОшибка запуска ядра поддержки виртуального защищенного принтера."),
                eId_ServiceNotStarted, VPrn::Error );




    QObject::connect (db_gateway,    SIGNAL(sendEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory)),
                      eLogger,       SLOT  (saveEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory))
                      );

    // Настройка связи полученных сообщений от клиентов к диспетчеру сообщений

    QObject::connect (locServer,     SIGNAL( newClientConnected(QString)) ,
                      db_gateway, SLOT  ( createNewClientAccount(QString)  )
                      );
    QObject::connect (locServer,     SIGNAL(currentClientDisConnected(QString)) ,
                      db_gateway, SLOT  ( deleteClientAccount(QString)  )
                      );
*/


}

void ControlSrv::pause()
{
    locServer->pause();
    eLogger->saveEventMessageInfo(
                QObject::trUtf8("\nЯдро поддержки виртуального защищенного принтера приостановленно."),
                eId_ServicePaused, VPrn::Information );
}

void ControlSrv::resume()
{
    locServer->resume();
    eLogger->saveEventMessageInfo(
                QObject::trUtf8("\nВозобновление работы ядра поддержки виртуального защищенного принтера."),
                eId_ServiceResume, VPrn::Information );
}

//---------------------------------- private -----------------------------------

bool ControlSrv::readConfig()
{
    bool Ok = true;
    {
        // Читаем файл настроек
        QString ini_path =QString("%1/controlsrv.ini")
                .arg(qApp->applicationDirPath() );

        if (QFile::exists(ini_path)){
            QSettings settings (ini_path,QSettings::IniFormat);

            settings.beginGroup("SERVICE");
            //serverName = settings.value("server").toString();
            Port = settings.value("port").toInt();
            settings.endGroup();

        }else{
            Ok  = false;
            eLogger->saveEventMessageInfo(
                        QObject::trUtf8("Файл [%1] с настройками программы не найден!")
                        .arg(ini_path),
                        eId_FileNotFound, VPrn::Error );
        }
    }
    return Ok;
}

