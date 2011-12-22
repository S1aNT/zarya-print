#include "controller.h"
#include <QtCore/QPluginLoader>
#include <QtCore/QSettings>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QFileInfoList>
#include <QModelIndex>
#include <QDateTime>


Controller::Controller(QObject*parent)
        :QObject(parent)
        , myGs_plugin(0)
        , myAuth_plugin(0)
        , m_netClient(0)
        , tDataGW(0)
        , serverName(QString())
        , serverPort(-1)
        , gsBin(QString())
        , pdftkBin(QString())
        , spoolDir(QString())
        , localTDir(QString())
        , globalTDir(QString())
        , ready4Print(false)
        , preViewMode(2)
{
    stampListModel = new QStringListModel(this);
    pictureListModel= new QStringListModel(this);

    cardModel = new QStandardItemModel(this);
    // Создадим запись - карточку документа именно с ней и будет работать клиент
    cardModel->setItem(0,VPrn::cards_STATUS,new QStandardItem(QObject::trUtf8("CREATE")));

    connect (cardModel,SIGNAL(itemChanged(QStandardItem*)),
             this,     SLOT  (itemChangedCardsModel(QStandardItem*))
             );

    printerModel = new QStandardItemModel(this);
    printerModel->setHorizontalHeaderItem( VPrn::printer_id,
                                           new QStandardItem(QObject::trUtf8("ID")));
    printerModel->setHorizontalHeaderItem( VPrn::printer_Name,
                                           new QStandardItem(QObject::trUtf8("Название принтера")));

    templatesModel = new QStandardItemModel(this);

    tDataGW = new TemplatesDataGateway(this);
    tDataGW->setTmplModel(templatesModel);
    tDataGW->setCardModel(cardModel);


#ifdef D_MYDEBUG
    m_netClient = new FakeNetAns();
#else
    m_netClient = new NetWorkClient();
#endif
    connect (m_netClient, SIGNAL( reciveMessage(const Message &)),
             this   , SLOT  ( parseNetworkMessage(const Message &))
             );

    connect (m_netClient, SIGNAL ( connectToRemoteServer()),
             this       , SLOT ( doConnect())
             );
    connect(tDataGW,SIGNAL(sendEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory)),
            this, SIGNAL(sendEventMessageInfo(QString,VPrn::EventLogMessageId,VPrn::EventLogType,VPrn::EventLogCategory))
            );     
}

Controller::~Controller()
{
    /// @todo До деструктора не доходит при завершении программы
    if (myGs_plugin){
        myGs_plugin->clear();
    }
}

bool Controller::init(const QString &app_dir,const QString &wFile)
{
    workFile = wFile;
    bool ok = true;
    {
        ok &= QFile::exists(workFile);
        if (ok )  {
            emit finishStep(VPrn::step_inputFile);
            ok &= (m_netClient && this->readConfig(app_dir));
            if (ok) {
                m_netClient->connectToHost(serverName,serverPort);
                this->findTemplates(localTDir);
                this->findTemplates(globalTDir);

                tDataGW->getMetaInfo(templatesList);

                ok &= this->loadPlugins() ;
                if (ok){
                    myAuth_plugin->init();
                    myGs_plugin->init(gsBin,pdftkBin,spoolDir);
                }
            }
        }
    }
    return ok;
}

void Controller::clearPreViewFiles()
{
    /// @todo Запрос к плагину gs_plugin за спискои файлов и их рекурсивное удаление
}

void Controller::convertTemplateToPdf()
{
    tDataGW->prepareTemplate(currentTemplate);
}

//------------------------- Public slots  --------------------------------------

void Controller::getNextCopy(int p_copy)
{
    QMap <int,QString> orderList = myGs_plugin->getOrderList(p_copy);
    if (!orderList.isEmpty()){
        int copy = cardModel->data( cardModel->index(0,VPrn::cards_PAGE_COUNT), Qt::EditRole).toInt();
        if (copy <1){
            emit showPrnState(QObject::trUtf8("В исходном документе ошибка, число страниц не может быть < 0."));
            return;
        }
        if ( copy > 1 ){
            copy--;
        }

        emit showPrnState(QObject::trUtf8("Начата печать экземпляра №%1").arg(p_copy,0,10));
        QMapIterator<int, QString> i(orderList);
        QString infoStr;
        while (i.hasNext()) {
            i.next();
            switch (i.key()){
            case 0:
                infoStr = QObject::trUtf8("Лицевая сторона первого листа отправлена на печать");
                break;
            case 1:
                infoStr = QObject::trUtf8("Лицевая сторона последующих листов отправлена на печать");
                break;
            case 2:
                infoStr = QObject::trUtf8("Обратная сторона документа отправлена на печать");
                break;
            case 3:
                infoStr = QObject::trUtf8("Фонарик отправлен на печать");
                break;
            default:
                break;
            }
            if (!infoStr.isEmpty() && QFile::exists( i.value())){
                emit showPrnState(infoStr);
                this->printThisFile( i.value(),copy);
            }
        }
        emit showPrnState(QObject::trUtf8("Окончена печать экземпляра №%1").arg(p_copy,0,10));
        emit needMarkDoc();
    }
}

void Controller::genFullPreView()
{
    /**
      *@brief Для печати документа формируем пакет печати для каждого варианта,
      * который включает в себя:
      *@li Первые страницы + последующие документа объединенный в один файл (опционально)
      *@li Обратные страницы документа (опционально)
      *@li Фонарик документа (опционально)
      */    
    preViewMode = 1;
}

void Controller::genPartPreView()
{        
    preViewMode = 2;
}

//------------------------- Private slots --------------------------------------
void Controller::parseNetworkMessage(const Message & msg)
{
    QString s_data;
    s_data.append(msg.getMessageData());

    switch (msg.getType()){
    case VPrn::Ans_AuthPassed:
        emit finishStep(VPrn::step_authPassed);
        break;
    case VPrn::Ans_StampList:
        stampListModel->setStringList(s_data.split(";:;"));

        if (stampListModel->rowCount() != -1){
            emit finishStep(VPrn::step_reciveSecLabel);
        }
        break;
    case VPrn::Ans_PrinterList:

        if (isSavePrinterToModel( msg.getMessageData() ) ){
            emit finishStep(VPrn::step_recivePrintersList);
        }
        break;
    case VPrn::Ans_CheckDocInBase:
        emit setCardValid(true);
        break;
    default:
        break;
    }
}

void Controller::doConnect()
{
    emit finishStep(VPrn::step_connect2RemoteDemon);
    // Формируем сообщение для проверки на УС
    Message msg;
    msg.setType(VPrn::Que_AuthUser);
    QString body = QObject::trUtf8("%1;:;%2;:;%3;:;%4").arg( m_user,m_role,m_mls,m_mcs);
    msg.setMessageData( body.toUtf8() );
    m_netClient->reSendNetworkMessage(msg);

}

void Controller::saveAuth(const QString &user,const QString &role,
                          const QString &mlsRange,const QString &mcsRange)
{
    m_user = user;
    m_role = role;
    m_mls  = mlsRange;
    m_mcs  = mcsRange;
    if (!m_user.isEmpty() && !m_role.isEmpty() && !m_mls.isEmpty() &&  ! m_mcs.isEmpty() ){
        emit finishStep(VPrn::step_defineAuthData);
    }
}

void Controller::gsPluginReady(const QString &wDir)
{
    myGs_plugin->convertPs2Pdf(workFile);
    workingDir =wDir;
    tDataGW->setWorkingDir( workingDir);
}

void Controller::docReady4work(qint32 pCount)
{
    /**
      * @fixme
      * @todo Возникает странное как только выполняется setItem модель очищается
      * т.е те значения которые ввел пользолватель пропадают
      */
    cardModel->setItem(0,VPrn::cards_PAGE_COUNT,new QStandardItem(QObject::trUtf8("%1").arg(pCount,0,10)));
}

void Controller::docConvertedToPdf()
{
    cardModel->setItem(0,VPrn::cards_CNV2PDF,new QStandardItem(QObject::trUtf8("1")));
}

void Controller::docMergedWithTemplate()
{
     /** @todo  Этому место в плагине, так как режим предпросмотра задается,
       * там же можно и определять нужно ли преобразование в картинки
       */
    switch (preViewMode){
    case 1:
        myGs_plugin->convertPdf2Png(true);
        break;
    case 2:
        myGs_plugin->convertPdf2Png(false);           
        break;
    }
}

void Controller::generatePreViewFinished()
{
    /** @todo Вынести в плагин всю логику обхода каталогов, возвращать только QStringList
      * содержащий список файлов
      */
    QStringList pic_files;
    for (int p_copy=VPrn::FirstPage; p_copy <= VPrn::FirstPageN5;p_copy++){
        QString copy_dir = QObject::trUtf8("%1/%2-copy").arg(workingDir).arg(p_copy,0,10);

#if QT_VERSION > 0x040402
        pic_files.append( myGs_plugin->findFilesInDir(copy_dir,QStringList()<<"*.png") );
#else
        pic_files << myGs_plugin->findFilesInDir(copy_dir,QStringList()<<"*.png") ;
#endif
    }
    pictureListModel->setStringList(pic_files);
    emit needUpdatePicturesList();

}

void Controller::itemChangedCardsModel(QStandardItem* item)
{
    /** @todo Проверить необходимость этих переменных, данные можно брать из модели */
    QModelIndex idx = cardModel->indexFromItem(item);
    if (idx.isValid()){
        int comboBoxId = cardModel->data(idx,Qt::EditRole).toInt();
        if (comboBoxId != -1){
            switch (idx.column()){
            case VPrn::cards_STAMP:
                {
                    QModelIndex i_fname = stampListModel->index(comboBoxId);
                    currentStamp = stampListModel->data( i_fname, Qt::EditRole).toString() ;
                    cardModel->setItem(0,VPrn::cards_STAMP_NAME,new QStandardItem(currentStamp) );
                }
                break;
            case VPrn::cards_PRINTER_NAME:
                {
                    QModelIndex name_idx = printerModel->index(comboBoxId,VPrn::printer_Name);
                    currentPrinter = printerModel->data( name_idx, Qt::EditRole).toString() ;
                }
                break;
            case VPrn::cards_TEMPLATE_NAME:
                {
                    QModelIndex name_idx = templatesModel->index(comboBoxId,VPrn::metaInfo_fname);
                    currentTemplate = templatesModel->data( name_idx, Qt::EditRole).toString() ;
                }
                break;

            default:
                break;
            }
        }

    }
}

//------------------------- Private --------------------------------------------
bool Controller::readConfig(const QString &app_dir)
{
    QString ini_file =QString("%1/zarya_prn.ini").arg(app_dir);
    if (QFile::exists(ini_file)){
        QSettings settings (ini_file,QSettings::IniFormat);

        settings.beginGroup("SERVICE");
        serverName      = settings.value("server").toString();
        serverPort      = settings.value("port").toInt();
        settings.endGroup();

        settings.beginGroup("FILES");
        gsBin = settings.value("gs").toString();
        pdftkBin = settings.value("pdftk").toString();
        settings.endGroup();

        settings.beginGroup("SPOOL");
        spoolDir       = settings.value("spool").toString();
        settings.endGroup();

        settings.beginGroup("TEMPLATES");
        localTDir  = settings.value("local_templates").toString();
        globalTDir = settings.value("global_templates").toString();
        settings.endGroup();

    }else{
        emit sendEventMessageInfo( QObject::trUtf8("Файл с настройками программы %1 не найден!\n%2")
                                   .arg(ini_file)
                                   .arg(QString(Q_FUNC_INFO)),
                                   VPrn::eId_BrokenConfigFile,
                                   VPrn::Error
                                   );
    }
    QDir testDir;

    bool ok = !serverName.isEmpty() &&  !serverPort !=-1 &&
              QFile::exists(gsBin) && QFile::exists(pdftkBin) &&
              testDir.exists(spoolDir) && testDir.exists(localTDir) &&
              testDir.exists(globalTDir);

    if (ok){
        return true;
    }

    emit sendEventMessageInfo( QObject::trUtf8("Файл с настройками программы %1 поврежден!")
                               .arg(ini_file),
                               VPrn::eId_BrokenConfigFile,
                               VPrn::Error
                               );
    return false;
}

bool Controller::loadPlugins()
{
    QDir pluginsDir(qApp->applicationDirPath());
    Igs_plugin   *gs_plugin_Interface;
    IAuth_plugin *auth_plugin_Interface;



    if (pluginsDir.dirName().toLower() == "debug" ||
        pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();

#if defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)){
        QPluginLoader pluginManager(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginManager.instance();
        bool needUnlugin = true;
        {
            if (!myGs_plugin){
                /// Загрузка плагина работы с ghostscript
                gs_plugin_Interface = qobject_cast<Igs_plugin *> (plugin);
                if (gs_plugin_Interface){
                    needUnlugin = false;
                    myGs_plugin = gs_plugin_Interface;

                    connect(plugin,SIGNAL(pluginReady(const QString &)),
                            this, SLOT(gsPluginReady(const QString &))
                            );
                    connect(plugin,SIGNAL(docReady4work( qint32 )),
                            this, SLOT(docReady4work(qint32))
                            );
                    connect(plugin,SIGNAL(docReady4print( )),
                            this, SLOT(setReady4Print())
                            );
                    connect(plugin,SIGNAL(docConvertedToPdf( )),
                            this, SLOT(docConvertedToPdf())
                            );
                    connect(plugin,SIGNAL(docMergedWithTemplate( )),
                            this, SLOT(docMergedWithTemplate())
                            );
                    connect(plugin,SIGNAL(generatePreViewFinished( )),
                            this, SLOT(generatePreViewFinished())
                            );
                    connect(tDataGW,SIGNAL(finishGenTemplate()),
                            plugin, SLOT(mergeWithTemplate())
                            );


                }
            }
            if (!myAuth_plugin){
                /// Загрузка плагина работы с selinux
                auth_plugin_Interface = qobject_cast<IAuth_plugin *> (plugin);
                if (auth_plugin_Interface){
                    needUnlugin = false;
                    myAuth_plugin = auth_plugin_Interface;
                    connect(plugin,SIGNAL(userAuth(const QString &,const QString &,const QString &,const QString &)),
                            this, SLOT(saveAuth(const QString &,const QString &,const QString &,const QString &))
                            );
                }
            }

        }
        if ( needUnlugin ){
            // Выгрузим его нафиг не наш он плагин, сто пудово :)
            qDebug() << QObject::trUtf8("Plugin's [%1] unload").arg(pluginsDir.absoluteFilePath(fileName));
            pluginManager.unload();
        }
    }
    if (myGs_plugin){
        emit finishStep(VPrn::step_loadPlugin);
        return true;
    }
    return false;
}

bool Controller::isSavePrinterToModel (QByteArray data)
{
    qint32 prn_count;

    QDataStream in (&data,QIODevice::ReadOnly);
    in >> prn_count;
    //printerModel->setRowCount(prn_count);
    //printerModel->setColumnCount(2);

    for (int i=0; i<prn_count; i++){
        QString p_id;
        QString p_name;

        in >>  p_id;
        in >>  p_name;

        printerModel->setItem(i,0,new QStandardItem( p_id ) );
        printerModel->setItem(i,1,new QStandardItem( p_name) );

    }
    if (printerModel->rowCount() !=-1){
        return true;
    }
    return false;
}

void Controller::findTemplates(const QString &t_path)
{
    if (t_path.isEmpty()){
        return;
    }

    QStringList filters;
    QDir dir;

    filters << "*.tmpl" << "*.TMPL";
    // Читаем шаблоны
    dir.setPath(t_path);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setNameFilters(filters);
    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {
        templatesList.append( list.at(i).absoluteFilePath() );
    }

}

void Controller::printThisFile(const QString &fileName,int copyes)
{
    QByteArray msg_body;
    QByteArray file_data;
    qint32     file_size = getCompresedFile(fileName,file_data);

    //Формируем файл для печати сжатый
    Message net_msg(this);
    net_msg.setType(VPrn::Que_PrintThisFile);
    /**
                      * @short Que_PrintThisFile Печать файла на выбранный пользователем принтер
                      * @param (QString) JobID (уникально для каждого экз. документа)
                      * @param (QString) Имя принтера (очереди печати на CUPS)
                      * @param (qint32)   copy_number  число копий 1-100
                      * @param (QString) user_name    имя пользователя
                      * @param (QString) job_title    имя задания
                      * @param (qint32) array_size   размер не сжатого буфера
                      * @param (QByteArray) файл для печати в формате QByteArray (сжатый)
                      */
    QDataStream out(&msg_body, QIODevice::WriteOnly );
    out.setVersion(QDataStream::Qt_4_0);

    /** @todo как правильно записать S1:S15:C0-C1024 */
    //Уникальный номер представляет собой QDateTime
    out     << QDateTime::currentDateTime().toTime_t()
            << cardModel->data( cardModel->index(0,VPrn::cards_MB_NUMBER), Qt::EditRole).toString()
            << cardModel->data( cardModel->index(0,VPrn::cards_PRINTER_NAME), Qt::EditRole).toString()
            << copyes
            << QObject::trUtf8("%1:%2:%3:%4").arg( m_user,m_role,m_mls,m_mcs)
            << cardModel->data( cardModel->index(0,VPrn::cards_DOC_NAME), Qt::EditRole).toString()
            << file_size
            << file_data;
    net_msg.setMessageData(msg_body);
    m_netClient->reSendNetworkMessage(net_msg);
}

qint32 Controller::getCompresedFile(const QString &fileName,
                                    QByteArray &data)
{
    data.clear();

    QFile file_in(fileName);
    if (!file_in.open(QIODevice::ReadOnly) ){
        return 0;
    }
    data = qCompress( file_in.readAll() );
    file_in.close();
//    QByteArray byteArray_in = file_in.readAll();
//    data = qCompress(byteArray_in);
    /** @todo возможна ошибка так как файл может быть qint64 а буфер не больше int*/
    return data.size();
}
