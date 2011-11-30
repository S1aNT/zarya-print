#include "db_gateway.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQueryModel>
#include <QDateTime>
#include <QFile>

Db_GateWay::Db_GateWay(QObject *parent)
    : QObject(parent)
    , m_DriverValid(false)
{


}

Db_GateWay::~Db_GateWay()
{
    {
        QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
        if (db.isOpen()){
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(QLatin1String("MAIN_CON"));
}

bool Db_GateWay::openMainBase(const QString &base_file)
{
    m_dbFile = base_file;    
    bool ok =true;
    {
        // Инициализация БД, проверка наличия требуемого драйвера
        QSqlDatabase db  = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"),
                                                     QLatin1String("MAIN_CON"));

        if (db.driver()->lastError().type() != QSqlError::ConnectionError) {
            emit sendEventMessageInfo(QObject::trUtf8("Драйвер sqlite, успешно загружен."),
                                      VPrn::eId_SQL_CoreInited,
                                      VPrn::Information,
                                      VPrn::eCatId_DebugInfo);

            m_DriverValid =true;
            db.setDatabaseName(m_dbFile);
            ok &=db.open() && initDB(db);// Настройка базы
            if (ok){
                // Проверим БД содержит нужные таблицы
                QStringList tables = db.tables();

                if (!(tables.contains("printers", Qt::CaseInsensitive) &&
                      tables.contains("users", Qt::CaseInsensitive) &&
                      tables.contains("rel_prn_usr", Qt::CaseInsensitive) &&
                      tables.contains("s_part", Qt::CaseInsensitive) &&
                      tables.contains("c_part", Qt::CaseInsensitive)
                      )){
                    //Создадим нужные таблицы
                    ok &= createMainDB();
                    qDebug() << Q_FUNC_INFO << " db tables list " << db.tables();
                    ok &= fillSTable();
                    ok &= fillCTable();
                }
            }else{
                ok &=false;
                emit sendEventMessageInfo(QObject::trUtf8("Ошибка открытия БД!\n%1")
                                          .arg(m_dbFile ),
                                          VPrn::eId_AppsError,
                                          VPrn::Error,
                                          VPrn::eCatId_Error);
            }
        }else{
            ok &=false;
            emit sendEventMessageInfo(QObject::trUtf8("Не могу загрузить драйвер sqlite!\n%1")
                                      .arg(QString(Q_FUNC_INFO)),
                                      VPrn::eId_SQL_CoreNotInited,
                                      VPrn::Error,
                                      VPrn::eCatId_Error);
        }
    }
    return ok;
}
//

//------------------------ PUBLIC SLOTS -----------------------------------------------
void Db_GateWay::saveUserToBase(const QString &c_uuid,QStringList & u_list)
{
    emit userListSaved(c_uuid,true);
    return;

    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
    if (db.isOpen()){
        QSqlQuery query (db);
        bool ok=true;
        {
            // step 1
            ok &= query.exec(QLatin1String("delete from input"));
            // Вставка в промежуточную таблицу
            ok &= query.prepare("INSERT INTO input (i_name) VALUES(?);");
            if (ok){
                for (int i=0; i<u_list.count();i++){
                    query.addBindValue(u_list.at(i));
                }
                ok &=query.exec();
                if (ok){ // SQL магия

                    QString sub_query =QObject::tr("DELETE %1 WHERE %2 IN (  "
                                                   " SELECT %2 FROM users    "
                                                   "   WHERE usr_name NOT IN "
                                                   "   (SELECT i_name FROM input)"
                                                   "                       ) ");

                    // step 1
                    ok &= query.exec(sub_query.arg("rel_prn_usr","usr_id"));
                    if (ok){
                        // step 2
                        ok &= query.exec(sub_query.arg("users","usr_name"));
                        if (ok){
                            // step 3
                            ok &= query.exec(QObject::tr("INSERT INTO users (usr_name) "
                                                         " SELECT i_name FROM input "
                                                         " WHERE i_name NOT IN "
                                                         " (SELECT usr_name from users)") );
                            // Обновим таблицу синхронизации
                            ok &= query.exec(QObject::tr("UPDATE sync SET usr='%1'").arg(QDateTime::currentDateTime().toTime_t(),0,10));

                            emit userListSaved(c_uuid,ok); //Завершили запись в БД пользователей


                        }else{
                            dumpError(query.lastError());
                        }
                    }else{
                        dumpError(query.lastError());
                    }
                }else{
                    dumpError(query.lastError());
                }
            }
        }
    }
}

void Db_GateWay::closeMainDb()
{
    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
    {
        if (db.isOpen()){
            db.commit();
            db.close();
        }
    }
}

void Db_GateWay::getDataBaseSlice(const QString &c_uuid,
                                  const QString &login,const QString &mandat)
{
    // Формирум поисковые части
    QRegExp rx("S(\\d{1,2}):C(\\d{1,4})");
    QString S,C;

    if ( login.isEmpty() || mandat.isEmpty() ){
        return;
    }
    if ( rx.indexIn(mandat ) != -1){
        S = QString("S%1").arg(rx.cap(1) );
        C = QString("C%1").arg(rx.cap(2) );
    }
    if ( S.isEmpty() || C.isEmpty() ){
        return;
    }   

    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
    {
        if (db.isOpen()){
            // Получили значения id соответствующие значению S или C
            int s_id   = getPartTableId(db,"s_part",S);
            int c_id   = getPartTableId(db,"c_part",C);

            if ( getUserId(db,login,s_id,c_id) ==-1){
                QString info = QObject::trUtf8("Пользователь с логином [%1] и мандатом [%2]  в базе отсутствует!")
                        .arg(login,mandat);
                emit informationToClient(c_uuid,info);
                return;
            }

            // Sql query ->QByteArray
            QSqlQueryModel *sqlModel = new QSqlQueryModel(this);


            QByteArray data;
            QDataStream out (&data, QIODevice::WriteOnly );
            out.setVersion(QDataStream::Qt_4_0);          

            out << QObject::trUtf8("printers");
            qDebug() << QObject::trUtf8("printers");
            //Получим все принтеры имеющие такой же или  ниже мандат

            QString sql = QObject::tr("SELECT id,prn_name,mandat_s_id,mandat_c_id, prn_desc "
                                      " FROM printers WHERE mandat_s_id <='%1' AND "
                                      " mandat_c_id <='%2'").arg(s_id,0,10).arg(c_id,0,10);
            sqlModel->setQuery(sql,db);
            if ( sqlModel->lastError().isValid() ){
                dumpError(sqlModel->lastError());
                emit informationToClient(c_uuid,QObject::trUtf8("Ошибка при выполнении sql запроса.\n Выбор принтеров имеющих такой и ниже мандат"));
            }else{
                qDebug() << "rowCount " << sqlModel->rowCount();
                qDebug() << "columnCount " << sqlModel->columnCount();

                out << sqlModel->rowCount();
                out << sqlModel->columnCount();
                for (int i=0; i < sqlModel->rowCount(); i++){
                    out << sqlModel->data(sqlModel->index(i, 0)); //id
                    out << sqlModel->data(sqlModel->index(i, 1)); //prn_name
                    out << sqlModel->data(sqlModel->index(i, 2)); //mandat_s_id
                    out << sqlModel->data(sqlModel->index(i, 3)); //mandat_c_id
                    out << sqlModel->data(sqlModel->index(i, 4)); //prn_desc
                }
                out << QObject::trUtf8("users");
                qDebug() << QObject::trUtf8("users");
                //sqlModel->clear();
                sqlModel->setQuery(QObject::tr("SELECT id,usr_name, mandat_s_id,mandat_c_id FROM users"),db);
                if ( sqlModel->lastError().isValid() ){
                    dumpError(sqlModel->lastError());
                    emit informationToClient(c_uuid,QObject::trUtf8("Ошибка при выполнении sql запроса.\n Выбор списка пользователей."));
                }else{
                    qDebug() << "rowCount " << sqlModel->rowCount();
                    qDebug() << "columnCount " << sqlModel->columnCount();
                    out << sqlModel->rowCount();
                    out << sqlModel->columnCount();
                    for (int i=0; i < sqlModel->rowCount(); i++){
                        out << sqlModel->data(sqlModel->index(i, 0)); //id
                        out << sqlModel->data(sqlModel->index(i, 1)); //usr_name
                        out << sqlModel->data(sqlModel->index(i, 2)); //mandat_s_id
                        out << sqlModel->data(sqlModel->index(i, 3)); //mandat_c_id
                    }
                    out << QObject::trUtf8("rel_prn_usr");
                    qDebug() << QObject::trUtf8("rel_prn_usr");
                    //sqlModel->clear();
                    sqlModel->setQuery(QObject::tr("SELECT prn_id,usr_id FROM rel_prn_usr"
                                                   " WHERE prn_id IN "
                                                   "(SELECT id FROM printers WHERE mandat_s_id <='%1' AND "
                                                   " mandat_c_id <='%2')").arg(s_id,0,10).arg(c_id,0,10),
                                       db);
                    if ( sqlModel->lastError().isValid() ){
                        dumpError(sqlModel->lastError());
                        emit informationToClient(c_uuid,QObject::trUtf8("Ошибка при выполнении sql запроса.\n Выбор расширенных атрибутов принтеров"));
                    }else{
                        qDebug() << "rowCount " << sqlModel->rowCount();
                        qDebug() << "columnCount " << sqlModel->columnCount();
                        out << sqlModel->rowCount();
                        out << sqlModel->columnCount();
                        for (int i=0; i < sqlModel->rowCount(); i++){
                            out << sqlModel->data(sqlModel->index(i, 0)); //prn_id
                            out << sqlModel->data(sqlModel->index(i, 1)); //usr_id
                        }
                    }
                    emit setDataBaseSlice (c_uuid,data);
                }
            }
            sqlModel->deleteLater();

        }
    }
}
//-------------------------- Private ------------------------------------------


int Db_GateWay::getPartTableId(QSqlDatabase db,const QString &table, const QString &value)
{
    int id(-1);
    QSqlQuery query(db);

    qDebug() << QObject::tr("SELECT \"id\" FROM \"%1\" WHERE display_val=\"%3\"").arg(table,value);

    if ( query.exec( QObject::tr("SELECT \"id\" FROM \"%1\" WHERE display_val=\"%3\"").arg(table,value) ) ){
        int field_id  = query.record().indexOf("id");
        query.next();
        id = query.value(field_id).toInt();
    }else{
        dumpError(query.lastError());
    }
    return id;
}

int Db_GateWay::getUserId(QSqlDatabase db,const QString &login, int s_id, int c_id)
{
    int id(-1);
    QSqlQuery query(db);
    QString sql_txt = QObject::tr("SELECT id FROM users "
                                  " WHERE usr_name ='%1' AND mandat_s_id='%2' AND mandat_c_id ='%3'")
            .arg(login).arg(s_id,0,10).arg(c_id,0,10) ;
    if ( query.exec( sql_txt ) ){
        int field_id  = query.record().indexOf("id");
        query.next();
        id = query.value(field_id).toInt();
    }else{
        dumpError(query.lastError());
    }
    return id;
}

bool Db_GateWay::initDB(QSqlDatabase db)
{
    QSqlQuery pragma (db);
    bool ok = pragma.exec (QLatin1String("PRAGMA journal_mode=TRUNCATE;"
                                         "PRAGMA synchronous=OFF;"
                                         "PRAGMA temp_store=MEMORY;"
                                         "PRAGMA foreign_keys=ON" ));
    if (!ok){
        dumpError(pragma.lastError());
    }
    return ok;
}

void Db_GateWay::dumpError (const QSqlError & lastError)
{
    emit sendEventMessageInfo(QObject::trUtf8("Ошибка работы с БД sqlite. %1")
                              .arg( lastError.text() ),
                              VPrn::eId_SQL_ExecError,
                              VPrn::Error );

    qDebug()<< "lastError.text() "        << lastError.text() << "\n"
            << "lastError.databaseText() "<< lastError.databaseText() << "\n"
            << "lastError.driverText()"   << lastError.driverText() << "\n"
            << "lastError.type() "        << lastError.type() << "\n"
            << "lastError.number() "      << lastError.number() << "\n";
}

bool Db_GateWay::createMainDB()
{ 
    bool ok=true;
    {        
        QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
        QSqlQuery query(db);
        ok &= query.exec (QLatin1String("PRAGMA journal_mode=TRUNCATE;"
                                        "PRAGMA synchronous=OFF;"
                                        "PRAGMA temp_store=MEMORY;"
                                        "PRAGMA foreign_keys=ON" ));
        if (ok){

            ok &= query.exec (QLatin1String("CREATE TABLE sync (prn INTEGER,usr INTEGER, atr INTEGER);"));
            if (!ok){
                dumpError(query.lastError());
            }
            ok &= query.exec (QLatin1String("create table input    (id INTEGER primary key autoincrement,i_name text,i_mandat_s INTEGER, i_mandat_c INTEGER);"));
            if (!ok){
                dumpError(query.lastError());
            }
            ok &= query.exec (QLatin1String("create table printers (id INTEGER primary key autoincrement,"
                                            "prn_name text,mandat_s_id INTEGER,mandat_c_id INTEGER, prn_desc text,"
                                            "FOREIGN KEY(mandat_s_id) REFERENCES s_part(id),"
                                            "FOREIGN KEY(mandat_c_id) REFERENCES c_part(id) );"));
            if (!ok){
                dumpError(query.lastError());
            }
            ok &= query.exec (QLatin1String("create table users    (id INTEGER primary key autoincrement,"
                                            "usr_name text,mandat_s_id INTEGER,mandat_c_id INTEGER,"
                                            "FOREIGN KEY(mandat_s_id) REFERENCES s_part(id),"
                                            "FOREIGN KEY(mandat_c_id) REFERENCES c_part(id) );"));
            if (!ok){
                dumpError(query.lastError());
            }
            ok &= query.exec (QLatin1String("create table rel_prn_usr (prn_id INTEGER,usr_id INTEGER,"
                                            "FOREIGN KEY(prn_id) REFERENCES printers(id),"
                                            "FOREIGN KEY(usr_id) REFERENCES users(id));"));
            if (!ok){
                dumpError(query.lastError());
            }

            ok &= query.exec (QLatin1String("create table s_part (id INTEGER primary key autoincrement, display_val text);"));
            if (!ok){
                dumpError(query.lastError());
            }
            ok &= query.exec (QLatin1String("create table c_part (id INTEGER primary key autoincrement, display_val text);"));
            if (!ok){
                dumpError(query.lastError());
            }

            qDebug() << Q_FUNC_INFO << " db tables list " << db.tables();
        }else{
            dumpError(query.lastError());
        }

    }

    return ok;
}

bool Db_GateWay::fillSTable()
{
    bool ok = true;
    {
        QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
        QSqlQuery query (db);

        ok &= query.exec("DELETE FROM s_part;");
        //Заполним таблицы S [0-15]  значениями
        if (ok){
            for (int i=1;i<16;i++){
                ok &= query.exec(QObject::tr("INSERT INTO \"s_part\" (display_val) VALUES('S%1');")
                                              .arg(i,0,10)
                                 );
                if (!ok){
                    dumpError(query.lastError());
                }
            }
        }else{
            dumpError(query.lastError());
        }
    }
    return ok;
}

bool Db_GateWay::fillCTable()
{
    bool ok = true;
    {
        QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
        QSqlQuery query (db);
        ok &= query.exec("DELETE FROM c_part;");
        //Заполним таблицы C[0-1024] значениями
        if (ok){
            db.transaction();
            for (int i=1;i<1025;i++){
                ok &= query.exec(QObject::tr("INSERT INTO \"c_part\" (display_val) VALUES('C%1');")
                                              .arg(i,0,10)
                                 );
                if (!ok){
                    dumpError(query.lastError());
                }
            }
            db.commit();
        }else{
            dumpError(query.lastError());
        }
    }
    return ok;
}

int Db_GateWay::compare(const QString &M1, const QString & M2)
{
    // Формирум поисковые части
    QRegExp rx("S(\\d{1,2}):C(\\d{1,4})");

    int S1(0),S2(0),C1(0),C2(0);

    if(rx.indexIn(M1) != -1){
        S1  = rx.cap(1).toInt();
        C1  = rx.cap(2).toInt();
    }

    if(rx.indexIn(M2) != -1){
        S2  = rx.cap(1).toInt();
        C2  = rx.cap(2).toInt();
    }

    if (S1 < S2){
        return -1;
    }else{
        if (S1 > S2 ){
            return 1;
        }else{
            if (S1 == S2 ){
                if ( C1 < C2 ){
                    return -1;
                }else{
                    if (C1 > C2){
                        return 1;
                    }
                }

            }
        }
    }
    return 0;
}




