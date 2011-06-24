#include "engine.h"
#include <QSettings>
#include <QLibrary>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QStandardItem>


Engine::Engine(QObject *parent)
    : QObject(parent)
    , data_model(0)
{

    data_model = new QStandardItemModel(this);
    QList <QStandardItem *> items;

    for (int i=0;i<9;i++){
        items.append(new QStandardItem());
    }
    data_model->appendRow(items);
/*
    data_model->setHeaderData(VPrn::bp_Login,Qt::Horizontal,QObject::trUtf8("Логин"));
    data_model->setHeaderData(VPrn::bp_Mandat,Qt::Horizontal,QObject::trUtf8("Мандат"));
    data_model->setHeaderData(VPrn::bp_NameZP,Qt::Horizontal,QObject::trUtf8("Имя ZP запроса"));
    data_model->setHeaderData(VPrn::bp_NameGM,Qt::Horizontal,QObject::trUtf8("Имя групой машины"));
    data_model->setHeaderData(VPrn::bp_DayM,Qt::Horizontal,QObject::trUtf8("ДеньМ"));
    data_model->setHeaderData(VPrn::bp_VRNT,Qt::Horizontal,QObject::trUtf8("Вариант"));
    data_model->setHeaderData(VPrn::bp_NBM ,Qt::Horizontal,QObject::trUtf8("Номер МБ"));
    data_model->setHeaderData(VPrn::bp_Req ,Qt::Horizontal,QObject::trUtf8("Номера запросов"));
    data_model->setHeaderData(VPrn::bp_Trace ,Qt::Horizontal,QObject::trUtf8("Протокол"));
    */
}

Engine::~Engine()
{
    if (data_model !=0){
        data_model->deleteLater();
    }
}

//----------------- Public Slots -----------------------------------------------
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
