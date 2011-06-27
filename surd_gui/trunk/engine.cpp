#include "engine.h"
#include <QSettings>
#include <QLibrary>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QStandardItem>
#include <QProcess>
#include <QDataStream>


#include "message.h"

Engine::Engine(QObject *parent)
    : QObject(parent)
    , data_model(0)
    ,prnInfoModel(0)
{
    prnInfoModel = new QStandardItemModel(this);

    data_model = new QStandardItemModel(5,4,this);

    data_model->setHorizontalHeaderLabels(QStringList()<< QObject::trUtf8("Принтеры") << QObject::trUtf8("Метка")<< QObject::trUtf8("Примечание") << tr("ID"));

    for (int nTopRow=0;nTopRow<5;++nTopRow){
        QModelIndex index=data_model->index(nTopRow,0);

        data_model->setData(data_model->index(nTopRow,0),"Printer_"+QString::number(nTopRow+1),Qt::DisplayRole);
        data_model->setData(data_model->index(nTopRow,0),QIcon(":/printer.png"),Qt::DecorationRole);

        data_model->setData(data_model->index(nTopRow,1),"S"+QString::number(nTopRow+1)+":C"+QString::number(nTopRow+20) );

        data_model->setData(data_model->index(nTopRow,3),QString::number(nTopRow+1),Qt::DisplayRole);

        data_model->insertRows(0,4,index);
        data_model->insertColumns(0,4,index);
        for (int nRow=0;nRow<4;++nRow){
            data_model->setData(data_model->index(nRow,0,index),QObject::trUtf8("Юзер"),Qt::DisplayRole);
            data_model->setData(data_model->index(nRow,0,index),QIcon(":/user.png"),Qt::DecorationRole);
            //data_model->itemFromIndex(data_model->index(nRow,0,index))->setCheckable(true);
            //data_model->itemFromIndex(data_model->index(nRow,0,index))->setCheckState(Qt::Checked);

            data_model->setData(data_model->index(nRow,1,index),"S"+QString::number(nRow+1)+":C"+QString::number(nRow+40) );
            data_model->setData(data_model->index(nRow,2,index),QObject::trUtf8("Примечание"),Qt::DisplayRole);
            data_model->setData(data_model->index(nRow,3,index),QString::number(nRow+1),Qt::DisplayRole);
        }
    }


}

Engine::~Engine()
{
    if (data_model !=0){
        data_model->deleteLater();
    }
    if (prnInfoModel!=0){
        prnInfoModel->deleteLater();
    }
}

//----------------- Public Slots -----------------------------------------------
void Engine::getUserId()
{
    QProcess proc;
    QString m_Output;

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
        //Разберем ответ в формате
        m_login = "usr_1";
        m_mandat= "S3:C245";

        emit definedAuthData(m_login,m_mandat);
    }

}

void Engine::parseNetworkMessage(const Message & msg)
{
    MessageType mType = msg.getType();
    QByteArray m_data = msg.getMessageData();

    switch (mType){
    case VPrn::Ans_GetPrintersInfo:
    {
        QDataStream in(&m_data, QIODevice::ReadOnly );
        in.setVersion(QDataStream::Qt_4_0);
        //in >> prnList;   // Получим список принтеров
        //in >> userList;  // Получим список пользователей
        //in >> relPrnUsr; // Получим таблицу связей Принтер->Допущенные поьзователи
    }
        break;
    default:
        break;
    }

}

//-------------------- Private --------------------------------------------------------------
void Engine::updateData()
{

}
/*
void Engine::setAuthData(const QString &login,const QString &mandat)
{
    QModelIndex index;
    // Запишем в модель логин и мандат
    index= data_model->index(1,VPrn::bp_Login);
    data_model->setData(index,login,Qt::EditRole);
    index = data_model->index(1,VPrn::bp_Mandat);
    data_model->setData(index,mandat,Qt::EditRole);
}
bool Engine::foundGMinList(const QString &f_gm)
{
    if ( m_gmlist.contains(f_gm,Qt::CaseInsensitive)){
        return true;
    }
    return false;
}


void Engine::getAuth(const QString &mandat_filename)
{
    QString user_name;
    QString user_mandat;

#if defined(Q_OS_UNIX)
    logon_user_name   = QString(getenv("USERNAME"));

    if (!QFile::exists(mandat_filename)) {
        // Проверим факт существования по указанному пути
        emit error(VPrn::FileIOError,
                   QObject::trUtf8("Error: Файл мандата [%1] не найден!\n%2")
                   .arg(mandat_filename)
                   .arg(QString(Q_FUNC_INFO)));

    }else{
        m_mandat_filename = mandat_filename;
        // Разборки в малом токио

        QFile ticket (m_mandat_filename);
        QString text;
        if (ticket.open(QIODevice::ReadOnly)) {
            QTextStream in_stream(&ticket);
            in_stream >> text;
        }
        ticket.close();

        // Разберем по кирпичику
        QStringList list = text.split(",");

        for (int i=0; i< list.size();i++){
            QRegExp rx("^(.+)=(.+)$");
            if(rx.indexIn(list.at(i)) != -1){
                QString val =rx.cap(1);
                QString key = rx.cap(2);
                if (val.compare("uid")==0){
                    user_name = key;
                }
                if (val.compare("mid")==0){
                    user_mandat = key;
                }
            }
        }
        if (user_name.isEmpty()){
            user_name = logon_user_name;
        }
    }
#elif defined(Q_OS_WIN)
    // Нет у меня другого выхода спросим у системы
    // определим из реестра путь к Logon User Name
    QSettings log_settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer",QSettings::NativeFormat);
    logon_user_name = log_settings.value("Logon User Name").toString();

    // проверка существования агента безопастности
    // определим из реестра путь к библиотеке ldap_plus.dll
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\pGina",QSettings::NativeFormat);
    plugin_path =settings.value("pathPlugin").toString(); // returns "full path"
    // Загрузка библиотеки
    QLibrary myLib;
    myLib.setFileName(plugin_path);
    myLib.load();
    //qDebug()<< "Library " << myLib.fileName();
    if (myLib.isLoaded()){
        DLLGETCURRENTUSER pluginGetUSER= (DLLGETCURRENTUSER) myLib.resolve("GetCurrentUser");
        DLLGETCURRENTSECLABEL pluginGetCurrentSecLabel=(DLLGETCURRENTSECLABEL) myLib.resolve("GetCurrentSecLabel");
        // Читаем данные через интерфейс к LDAP
        user_name =QString::fromUtf16((ushort*)pluginGetUSER());
        user_mandat =QString::fromUtf16((ushort*)pluginGetCurrentSecLabel());
    }

#endif
    // Проверка на русские буквы
    QByteArray buf = user_name.toLatin1();
    if (!buf.isEmpty() ){
        emit definedAuthData(user_name,user_mandat);

    }else{
        emit error(VPrn::AuthCommonError,
                   QObject::trUtf8("Ошибка преобразования имени пользователя %1 в Latin1.%2\n")
                   .arg(user_name)
                   .arg(QString(Q_FUNC_INFO))
                   );
    }
}

void Engine::readGMList(const QString &gm_file)
{
    if (QFile::exists(gm_file)){

        QFile textFile;
        textFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream textStream(&textFile);
        QString lines = textStream.readAll();
        m_gmlist = lines.split(QRegExp("(\\r\\n)|(\\n\\r)|\\r|\\n"), QString::SkipEmptyParts);
    }else{
        emit error(VPrn::FileNotFound,
                   QObject::trUtf8("Файл [%1] со списком групповых машин не найден!\n%2")
                               .arg(gm_file)
                               .arg(QString(Q_FUNC_INFO))
                               );
    }
}

void Engine::saveTraceState(bool st)
{
    QModelIndex index;
    QString st_str;
    index= data_model->index(1,VPrn::bp_Trace);
    if (st){
        st_str ="TRACE";
    }else{
        st_str ="NOTRACE";
    }
    data_model->setData(index,st_str,Qt::EditRole);
}

void Engine::checkingModel()
{
    QModelIndex index;
    for (int c=0;c< data_model->columnCount();c++){
        index= data_model->index(data_model->rowCount()-1,c);
        QString cell_data = data_model->data(index,Qt::DisplayRole).toString();
        if (cell_data.isEmpty()){
            data_model->setData(index,QString("."));
        }

    }
}

void Engine::modelToStringList(QStringList &s_data)
{
    // Формируем данные для миши
    //BPUSK_PLAIN toVMDoc;:;begin
    //BPUSK_PLAIN mandat;:;имя груповой машины
    //BPUSK_PLAIN :READ имя_ЗП_раздела REQ A1 логин TRACE номер_мб деньМ вариант
    //BPUSK_PLAIN 001
    //BPUSK_PLAIN 002
    //BPUSK_PLAIN 201
    //BPUSK_PLAIN toVMDoc;:;end

     QModelIndex i_login  = data_model->index(data_model->rowCount()-1,VPrn::bp_Login);
     QModelIndex i_mandat = data_model->index(data_model->rowCount()-1,VPrn::bp_Mandat);
     QModelIndex i_nameZP = data_model->index(data_model->rowCount()-1,VPrn::bp_NameZP);
     QModelIndex i_nameGM = data_model->index(data_model->rowCount()-1,VPrn::bp_NameGM);
     QModelIndex i_dayM   = data_model->index(data_model->rowCount()-1,VPrn::bp_DayM);
     QModelIndex i_vrnt   = data_model->index(data_model->rowCount()-1,VPrn::bp_VRNT);
     QModelIndex i_nbm    = data_model->index(data_model->rowCount()-1,VPrn::bp_NBM );
     QModelIndex i_req    = data_model->index(data_model->rowCount()-1,VPrn::bp_Req);
     QModelIndex i_trace  = data_model->index(data_model->rowCount()-1,VPrn::bp_Trace);


    s_data.append("BPUSK_PLAIN toVMDoc;:;begin");
    s_data.append(QString("BPUSK_PLAIN %1;:;%2")
                  .arg(data_model->data(i_mandat,Qt::DisplayRole).toString())
                  .arg(data_model->data(i_nameGM,Qt::DisplayRole).toString())
                  );
    s_data.append(QString("BPUSK_PLAIN :READ %1 REQ A1 %2 %3 %4 %5 %6")
                  .arg(data_model->data(i_nameZP,Qt::DisplayRole).toString())
                  .arg(data_model->data(i_login,Qt::DisplayRole).toString())
                  .arg(data_model->data(i_trace,Qt::DisplayRole).toString())
                  .arg(data_model->data(i_nbm,Qt::DisplayRole).toString())
                  .arg(data_model->data(i_dayM,Qt::DisplayRole).toString())
                  .arg(data_model->data(i_vrnt,Qt::DisplayRole).toString())
                  );
    //Формируем строчки запросов
    QStringList r_lines = data_model->data(i_req,Qt::DisplayRole).toString().split(",");
    for (int i=0;i<r_lines.count();i++){
        QString line =r_lines.at(i);
        QRegExp rx("\\d{1,3}\\-\\d{1,3}");
        if(rx.indexIn(line) != -1){
            // Разбираем на интервалы
            не правильно разобрал
            int left_brd  = rx.cap(1).toInt();
            int right_brd  = rx.cap(2).toInt();

            int minValue = qMin(left_brd,right_brd);
            int maxValue = qMax(left_brd,right_brd);

            for (int j=minValue;j<maxValue;j++){
                QString temp_line= QString("%1").arg(j,0,10);
                if (temp_line.size() ==1){
                    temp_line ="00"+temp_line;
                }else if (temp_line.size() ==2){
                    temp_line ="0"+temp_line;
                }
                s_data.append(QString("BPUSK_PLAIN %1").arg(temp_line));
            }
        }else{
            if (line.size() ==1){
                line ="00"+line;
            }else if (line.size() ==2){
                line ="0"+line;
            }
            s_data.append(QString("BPUSK_PLAIN %1").arg(line));
        }
    }
    s_data.append("BPUSK_PLAIN toVMDoc;:;end");

}
  */
