#include "db_gateway.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQueryModel>
#include <QDateTime>
#include <QFile>

Db_GateWay::Db_GateWay(QObject *parent)
    : QObject(parent)
    , prnModel(0)
    , usrModel(0)
    , prn_usrModel(0)
    , m_DriverValid(false)
{
    // Инициализация БД, проверка наличия требуемого драйвера
    {
        QSqlDatabase db  = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"),
                                                     QLatin1String("MAIN_CON"));
        if (db.driver()->lastError().type() != QSqlError::ConnectionError) {
            emit sendEventMessageInfo(QObject::trUtf8("Драйвер sqlite, успешно загружен."),
                                      VPrn::eId_SQL_CoreInited,
                                      VPrn::Information,
                                      VPrn::eCatId_DebugInfo);

            m_DriverValid =true;
        }else{
            emit sendEventMessageInfo(QObject::trUtf8("Не могу загрузить драйвер sqlite!\n%1")
                                      .arg(QString(Q_FUNC_INFO)),
                                      VPrn::eId_SQL_CoreNotInited,
                                      VPrn::Error,
                                      VPrn::eCatId_Error);
        }
    }

}

Db_GateWay::~Db_GateWay()
{
    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
    {
        if (db.isOpen()){
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(QLatin1String("MAIN_CON"));
}

bool Db_GateWay::openMainBase(const QString &base_file)
{
    m_dbFile = base_file;
    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
    db.setDatabaseName(m_dbFile);
    bool ok=true;
    {
        ok &= m_DriverValid;
        if (ok){
            //Создаем модели
            prnModel = new QSqlTableModel(this,db);
            usrModel = new QSqlTableModel(this,db);
            prn_usrModel = new QSqlTableModel(this,db);

            if (!QFile::exists(m_dbFile) ){ //File not found create empty base
                if (db.open() ){
                    ok &= createMainDB(db);
                }
            }
            if (db.open() ){

                //Проверим структуру базы
                QStringList tables = db.tables();
                ok &= ( tables.contains("printers", Qt::CaseInsensitive) &&
                       tables.contains("users", Qt::CaseInsensitive) &&
                       tables.contains("rel_prn_usr", Qt::CaseInsensitive)
                       );
                if (ok){
                    // Настройка базы
                    ok &= initDB(db);
                    /*
                    if (ok){
                        prnModel->setTable( "printers" );
                        prnModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
                        prnModel->select();
                        prnModel->setHeaderData(0, Qt::Horizontal, QObject::trUtf8("ID"));
                        prnModel->setHeaderData(1, Qt::Horizontal, QObject::trUtf8("Принтер"));
                        prnModel->setHeaderData(2, Qt::Horizontal, QObject::trUtf8("Мандат"));

                        usrModel->setTable( "users" );
                        usrModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
                        usrModel->select();
                        usrModel->setHeaderData(0, Qt::Horizontal, QObject::trUtf8("ID"));
                        usrModel->setHeaderData(1, Qt::Horizontal, QObject::trUtf8("Логин"));
                        usrModel->setHeaderData(2, Qt::Horizontal, QObject::trUtf8("Мандат"));


                        prn_usrModel->setTable( "rel_prn_usr" );
                        prn_usrModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
                        prn_usrModel->select();
                    }
                    */
                }
            }
        }
    }
    if (!ok){

        emit sendEventMessageInfo(QObject::trUtf8("Ошибка работы с БД!\n%1")
                                  .arg(db.lastError().text() ),
                                  VPrn::eId_SQL_ExecError,
                                  VPrn::Error,
                                  VPrn::eCatId_Error);

    }
    return ok;
}


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

void Db_GateWay::getBaseSlice(const QString &c_uuid,
                              const QString &login,const QString &mandat)
{
    // Формирум поисковые части
    QRegExp rx("S(\\d{1,2}):C(\\d{1,4})");

    if ( login.isEmpty() || mandat.isEmpty() ){
        return;
    }

    QString S,C;
    if(rx.indexIn(mandat) != -1){
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

            // Sql query -> model -> QByteArray
            QSqlQueryModel *sqlModel = new QSqlQueryModel(this);

            QByteArray data;
            QDataStream out (&data, QIODevice::ReadOnly );
            out.setVersion(QDataStream::Qt_4_0);          

            out << QObject::trUtf8("printers");
            //Получим все принтеры имеющие такой же или  ниже мандат

            QString sql = QObject::tr("SELECT id,prn_name,mandat_s_id,mandat_c_id, prn_desc "
                                      " FROM printers WHERE mandat_s_id <='%1' AND "
                                      " mandat_c_id <='%2'").arg(s_id,0,10).arg(c_id,0,10);
            sqlModel->setQuery(sql,db);
            if ( sqlModel->lastError().isValid() ){
                dumpError(sqlModel->lastError());
            }else{
                out << sqlModel->rowCount();
                out << sqlModel->columnCount();
                for (int i=0; i < sqlModel->rowCount(); i++){
                    out << sqlModel->data(sqlModel->index(i, 0)).toInt(); //id
                    out << sqlModel->data(sqlModel->index(i, 1)).toString();//prn_name
                    out << sqlModel->data(sqlModel->index(i, 2)).toInt(); //mandat_s_id
                    out << sqlModel->data(sqlModel->index(i, 3)).toInt(); //mandat_c_id
                    out << sqlModel->data(sqlModel->index(i, 4)).toString(); //prn_desc
                }
            }
            out << QObject::trUtf8("users");
            sqlModel->clear();
            sqlModel->setQuery(QObject::tr("SELECT id,usr_name, mandat_s_id,mandat_c_id FROM users"));
            if ( sqlModel->lastError().isValid() ){
                dumpError(sqlModel->lastError());
            }else{
                out << sqlModel->rowCount();
                out << sqlModel->columnCount();
                for (int i=0; i < sqlModel->rowCount(); i++){
                    out << sqlModel->data(sqlModel->index(i, 0)).toInt(); //id
                    out << sqlModel->data(sqlModel->index(i, 1)).toString();//usr_name
                    out << sqlModel->data(sqlModel->index(i, 2)).toInt(); //mandat_s_id
                    out << sqlModel->data(sqlModel->index(i, 3)).toInt(); //mandat_c_id
                }
            }
            sqlModel->clear();


            sqlModel->deleteLater();
        }
    }
}
//-------------------------- Private ------------------------------------------


int Db_GateWay::getPartTableId(QSqlDatabase db,const QString &table, const QString &value)
{
    int id(-1);
    QSqlQuery query(db);
    if ( query.exec( QObject::tr("SELECT id FROM %1 WHERE display_val='%3')").arg(table,value) ) ){
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
                                  " WHERE usr_name ='%1' AND mandat_s_id='%2' AND mandat_c_id ='%3')")
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

bool Db_GateWay::createMainDB(QSqlDatabase db)
{

    /*
                                          "create view prn_usr_attr as "
                                        " select printers.id,prn_name,prn_mandat,usr_name,usr_mandat from printers "
                                        " inner join rel_prn_usr on printers.id=rel_prn_usr.prn_id "
                                        " inner join users on users.id=rel_prn_usr.usr_id;"
*/
    bool ok=true;
    {
        QSqlQuery query(db);
        QString sql_txt = QLatin1String("BEGIN TRANSACTION;"
                                        "CREATE TABLE sync (prn INTEGER,usr INTEGER, atr INTEGER);"

                                        "create table input    (id INTEGER primary key autoincrement,"
                                        "i_name text,i_mandat_s INTEGER, i_mandat_c INTEGER);"

                                        "create table s_part   (id INTEGER primary key autoincrement,display_val text);"
                                        "create table c_part   (id INTEGER primary key autoincrement,display_val text);"

                                        "create table printers (prn_id INTEGER primary key autoincrement,"
                                        "prn_name text,mandat_s_id INTEGER,mandat_c_id INTEGER, prn_desc text,"
                                        "FOREIGN KEY(mandat_s_id) REFERENCES s_part(id),"
                                        "FOREIGN KEY(mandat_c_id) REFERENCES c_part(id) );"

                                        "create table users    (usr_id INTEGER primary key autoincrement,"
                                        "usr_name text,mandat_s_id INTEGER,mandat_c_id INTEGER,"
                                        "FOREIGN KEY(mandat_s_id) REFERENCES s_part(id),"
                                        "FOREIGN KEY(mandat_c_id) REFERENCES c_part(id) );"

                                        "create table rel_prn_usr (prn_id INTEGER,usr_id INTEGER,"
                                        "FOREIGN KEY(prn_id) REFERENCES printers(id),"
                                        "FOREIGN KEY(usr_id) REFERENCES users(id));"
                                        "COMMIT;"
                                        );
        ok &= query.exec (sql_txt);
        if (ok){
            //Заполним таблицы S [0-15]  значениями
            ok &= query.prepare("INSERT INTO s_part (display_val) VALUES(?);");
            if (ok){
                for (int i=0;i<15;i++){
                    query.addBindValue(QObject::tr("S%1").arg(i,0,10));
                }
                ok &= query.exec();
                if (ok){
                    //Заполним таблицы C[0-1024] значениями
                    ok &= query.prepare("INSERT INTO c_part (display_val) VALUES(?);");
                    if (ok){
                        for (int i=0;i<1024;i++){
                            query.addBindValue(QObject::tr("C%1").arg(i,0,10));
                        }
                        ok &= query.exec();
                        if (!ok){
                            dumpError(query.lastError());
                        }
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
        return ok;
    }
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




