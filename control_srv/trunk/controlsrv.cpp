#include "controlsrv.h"
#include <QSettings>
#include <QFile>

#include <QHostAddress>
#include "config.h"

ControlSrv::ControlSrv(int argc, char **argv)
    : QtService<QCoreApplication>(argc, argv, "CAPS_locServer")
    , eLogger(0)
    , locServer(0)
    , msgDispatcher(0)
    , db_gateway(0)
    , HostName (QString())
    , Port(0)
{    
    setServiceDescription("A Fast TCP/IP service need's to Virtual Safe Printer.");
    setServiceFlags(QtServiceBase::CanBeSuspended);  
    // Создаем зависимые объекты
    db_gateway = new Db_GateWay();
    eLogger = new ErrorLogger();    
    locServer = new LocalServer();
    msgDispatcher = new MsgDispatcher();

    // Все требуемые компонеты загружены, установим связи
    QObject::connect (locServer, SIGNAL(sendEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory)),
                      eLogger,   SLOT  (saveEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory))
                      );
    QObject::connect (locServer,     SIGNAL( reciveMessage(const QString &,const Message&) ),
                      msgDispatcher, SLOT  ( parseMessage(const QString &,const Message&)  )
                      );

    QObject::connect (msgDispatcher, SIGNAL(sendEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory)),
                      eLogger,       SLOT  (saveEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory))
                      );
    QObject::connect (msgDispatcher, SIGNAL(sendMsgToClient(const QString &,const Message &)),
                      locServer,     SLOT  (sendMessage(const QString &,const Message &))
                      );

    QObject::connect (msgDispatcher, SIGNAL( saveUserToBase(const QString &,QStringList &)),
                      db_gateway,    SLOT  ( saveUserToBase(const QString &,QStringList &) )
                      );
    QObject::connect (msgDispatcher, SIGNAL( getDataBaseSlice(const QString &,const QString&,const QString &)),
                      db_gateway,    SLOT  ( getDataBaseSlice(const QString &,const QString&,const QString &) )
                      );


    QObject::connect (db_gateway,    SIGNAL( userListSaved(const QString &,bool) ),
                      msgDispatcher, SLOT  ( userListSaved(const QString &,bool) )
                      );
    QObject::connect (db_gateway,    SIGNAL( setDataBaseSlice(const QString &,const QByteArray &) ),
                      msgDispatcher, SLOT  ( setDataBaseSlice(const QString &,const QByteArray &) )
                      );


}

// -------------------------- protected ----------------------------------------

void ControlSrv::start()
{
    QCoreApplication *app = application();

    //installLog("control_srv",QObject::trUtf8("Zarya"));

    // Чтение настроек приложения
    if ( eLogger && readConfig() && Port != 0 && !mainDB.isEmpty() ){

        QHostAddress adress;
        adress.setAddress(HostName);

        if (locServer->listen(adress,Port) ) {
            locServer->resume();
            eLogger->saveEventMessageInfo(
                        QObject::trUtf8("\nЯдро поддержки виртуального защищенного принтера запущенно."),
                        eId_ServiceStarted,
                        VPrn::Information
                        );
        }else{
            eLogger->saveEventMessageInfo(
                        QObject::trUtf8("Порт %1 уже занят другим приложением!")
                        .arg(locServer->serverPort()),
                        eId_CanNotBindPort, VPrn::Error  );
            app->exit(eId_CanNotBindPort);
        }
        if (db_gateway->openMainBase(mainDB)){

            eLogger->saveEventMessageInfo(
                        QObject::trUtf8("Инициализация основной БД [%1]!")
                        .arg(mainDB),
                        VPrn::eId_SQL_CoreInited, VPrn::Information  );

        }else{
            eLogger->saveEventMessageInfo(
                        QObject::trUtf8("Ошибка инициализации основной БД [%1]!")
                        .arg(mainDB),
                        VPrn::eId_SQL_CoreNotInited, VPrn::Error  );
            app->exit(VPrn::eId_SQL_CoreNotInited);
        }
    }else{
        eLogger->saveEventMessageInfo(
                    QObject::trUtf8("Ошибка чтения ini файла!"),
                    VPrn::eId_CanNotReadConfig, VPrn::Error );
        app->exit(VPrn::eId_CanNotReadConfig);
    }
}

void ControlSrv::pause()
{
    locServer->pause();
    eLogger->saveEventMessageInfo(
                QObject::trUtf8("Ядро поддержки виртуального защищенного принтера приостановленно."),
                eId_ServicePaused, VPrn::Information );
}

void ControlSrv::resume()
{
    locServer->resume();
    eLogger->saveEventMessageInfo(
                QObject::trUtf8("Возобновление работы ядра поддержки виртуального защищенного принтера."),
                eId_ServiceResume, VPrn::Information );
}

//---------------------------------- private -----------------------------------

bool ControlSrv::readConfig()
{
    bool Ok = true;
    {
        // Читаем файл настроек
        QString ini_path =QString("%1/zarya.ini")
                .arg(qApp->applicationDirPath() );

        if (QFile::exists(ini_path)){
            QSettings settings (ini_path,QSettings::IniFormat);

            settings.beginGroup("SERVICE");
            HostName = settings.value("server").toString();
            Port = settings.value("port").toInt();
            settings.endGroup();

            settings.beginGroup("FILES");
            mainDB = settings.value("main_db").toString();
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

