#include "localdb.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QtSql/QSqlField>
#include <QSqlDriver>
#include <QModelIndex>


LocalDB::LocalDB(QObject *parent)
    : QObject (parent)
    , prnModel(0)
    , usrModel(0)
    , relModel(0)
    , data_model(0)
    , conState(false)
{
    conState = createConnection();   
    data_model      = new QStandardItemModel(this);
/*
    data_model->setHorizontalHeaderLabels(QStringList()
                                          << QObject::trUtf8("Принтеры")
                                          << QObject::trUtf8("Метка")
                                          << tr("ID"));
*/
}

LocalDB::~LocalDB()
{
    if (prnModel){
        prnModel->deleteLater();
    }
    if (usrModel){
        usrModel->deleteLater();
    }
    if (relModel){
        relModel->deleteLater();
    }

    if (data_model !=0){
        data_model->deleteLater();
    }
}

QString LocalDB::getPrinterNameFromIndex(QModelIndex *index)
{
   //Определим сточку и столбец для индекса
    int i_row = index->row();
    QModelIndex idx = data_model->index(i_row,0);
    if ( idx.parent() != QModelIndex()){
        idx = idx.parent();
    }
    return data_model->data(idx,Qt::EditRole).toString();
}

QString LocalDB::getPrinterMandatFromIndex(QModelIndex *index)
{
   //Определим сточку и столбец для индекса


    QModelIndex idx = data_model->index(index->row(),0);
    if ( idx.parent() != QModelIndex()){
        idx = idx.parent();
    }
    int i_row = idx.row();
    int i_col = idx.column();
    return data_model->data(data_model->index(i_row,i_col+1),Qt::EditRole).toString();
}

void LocalDB::restoreDump(QByteArray *data)
{
    if ( !conState){
        emit sendEventMessageInfo(QObject::trUtf8("Ошибка работы с БД.Нет соединения с локальной базой данных!"),
                                  VPrn::eId_SQL_ExecError,
                                  VPrn::Error );
        return;
    }
    if (data->size() == -1 ) {
        emit sendEventMessageInfo(QObject::trUtf8("Упр. сервер не передал данные необходимые для работы приложения!"),
                                  VPrn::eId_AppsError,
                                  VPrn::Error );
        return;

    }

    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
    {                
        QSqlQuery query(db);

        QString rem;
        int row;
        int col;

        QDataStream in (data, QIODevice::ReadOnly );
        in.setVersion(QDataStream::Qt_4_0);

        in >> rem;
        in >> row;
        in >> col;

        query.prepare( QObject::tr("INSERT INTO \"%1\" VALUES(:idVal,:prn_nameVal,:s_idVal,:c_idVal,:descVal)").arg(rem) );
        for (int i=0; i < row; i++){
            QVariant cell;

            in >> cell;
            query.bindValue(":idVal", cell );
            in >> cell;
            query.bindValue(":prn_nameVal", cell );
            in >> cell;
            query.bindValue(":s_idVal", cell );
            in >> cell;
            query.bindValue(":c_idVal", cell );
            in >> cell;
            query.bindValue(":descVal", cell );

            if ( !query.exec()){
                dumpError(query.lastError());
                return;
            }
        }

        in >> rem;
        in >> row;
        in >> col;
        query.prepare( QObject::tr("INSERT INTO \"%1\" VALUES(:idVal,:usr_nameVal,:s_idVal,:c_idVal)").arg(rem) );
        for (int i=0; i < row; i++){
            QVariant cell;
            in >> cell;
            query.bindValue(":idVal", cell );
            in >> cell;
            query.bindValue(":usr_nameVal", cell );
            in >> cell;
            query.bindValue(":s_idVal", cell );
            in >> cell;
            query.bindValue(":c_idVal", cell );
            if ( !query.exec()){
                dumpError(query.lastError());
                return;
            }
        }

        in >> rem;
        in >> row;
        in >> col;

        query.prepare( QObject::tr("INSERT INTO \"%1\" VALUES(:prn_idVal,:usr_idVal)").arg(rem) );
        for (int i=0; i < row; i++){
            QVariant cell;
            in >> cell;
            query.bindValue(":prn_idVal", cell );
            in >> cell;
            query.bindValue(":usr_idVal", cell );

            if ( !query.exec()){
                dumpError(query.lastError());
                return;
            }
        }
        this->setData();
    }    

}

//---------------------------------- Private ----------------------------------------------------
bool LocalDB::createConnection()
{

    QSqlDatabase db  = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"),
                                                 QLatin1String("MAIN_CON"));
    if (db.driver()->lastError().type() != QSqlError::ConnectionError) {
        emit sendEventMessageInfo(QObject::trUtf8("Драйвер sqlite, успешно загружен."),
                                  VPrn::eId_SQL_CoreInited,
                                  VPrn::Information,
                                  VPrn::eCatId_DebugInfo);

        db.setDatabaseName(":memory:");

        bool ok=true;
        {
            ok &= db.open();
            QSqlQuery query(db);
            ok &= query.exec (QLatin1String("PRAGMA journal_mode=TRUNCATE;"
                                            "PRAGMA synchronous=OFF;"
                                            "PRAGMA temp_store=MEMORY;"
                                            "PRAGMA foreign_keys=ON" ));
            if (ok){
                ok &= createMainDB();
                ok &=fillSTable();
                ok &=fillCTable();

                /// @todo Может все таки она не нужна и можно обойтись запросом ?
                prnModel = new QSqlTableModel(this,db);
                prnModel->setTable("printers");
                usrModel = new QSqlTableModel(this,db);
                usrModel->setTable("users");
            }else{
                dumpError(query.lastError());
            }
        }
        return ok;
    }else{
        emit sendEventMessageInfo(QObject::trUtf8("Не могу загрузить драйвер sqlite!\n%1")
                                  .arg(QString(Q_FUNC_INFO)),
                                  VPrn::eId_SQL_CoreNotInited,
                                  VPrn::Error,
                                  VPrn::eCatId_Error);
    }
    return false;
}

void LocalDB::dumpError (const QSqlError & lastError)
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

void LocalDB::setData()
{
    if ( !conState){
        emit sendEventMessageInfo(QObject::trUtf8("Ошибка работы с БД.Нет соединения с локальной базой данных!"),
                                  VPrn::eId_SQL_ExecError,
                                  VPrn::Error );
        return;
    }
    /*
       data_model->index(i,0) == "Принтеры"   == prnModel->index(i,1)
       data_model->index(i,1) == "Метка"      == prnModel->index(i,2) ":" prnModel->index(i,3)
       data_model->index(i,2) == "ID"         == prnModel->index(i,0)

     */
    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
    {
        /*
        data_model->setHorizontalHeaderLabels(QStringList()
                                              << QObject::trUtf8("Принтеры")
                                              << QObject::trUtf8("Метка")
                                              << tr("ID"));
        */


        prnModel->select();
        data_model->insertRows(0,prnModel->rowCount());
        data_model->insertColumns(0,3);

        for (int i=0;i<prnModel->rowCount();i++){
            QModelIndex prn_index=data_model->index(i,0);

            int prn_id     = prnModel->data(prnModel->index(i,0),Qt::DisplayRole).toInt();

            // Формируем принтер с атрибутами

            data_model->setData(data_model->index(i,0),
                                prnModel->data(prnModel->index(i,1),Qt::DisplayRole),
                                Qt::DisplayRole);
            data_model->setData(data_model->index(i,0),QIcon(":/printer.png"),Qt::DecorationRole);

            data_model->setData(data_model->index(i,1),
                                getMandatFromId(prnModel->data(prnModel->index(i,2),Qt::DisplayRole).toInt(),
                                                prnModel->data(prnModel->index(i,3),Qt::DisplayRole).toInt()),
                                Qt::DisplayRole);

            data_model->setData(data_model->index(i,2),
                                prnModel->data(prnModel->index(i,0),Qt::DisplayRole),
                                Qt::DisplayRole);

            int user_count = getUsersCountFromPrinter(prn_id);
            if (user_count !=-1){
                QSqlQuery query(db);
                QString sql_txt = QObject::tr("SELECT id,usr_name,mandat_s_id,mandat_c_id "
                                              " FROM users WHERE id IN (SELECT usr_id FROM rel_prn_usr WHERE prn_id ='%1')"
                                              ).arg(prn_id,0,10);
                if ( query.exec( sql_txt ) ){
                    // Привяжем пользователей
                    data_model->insertRows(0,user_count,prn_index);
                    data_model->insertColumns(0,3,prn_index);
                    int nRow =0;
                    int idField   = query.record().indexOf("id");
                    int nameField = query.record().indexOf("usr_name");
                    int s_idFiled = query.record().indexOf("mandat_s_id");
                    int c_idFiled = query.record().indexOf("mandat_c_id");

                    while (query.next()){

                        data_model->setData(data_model->index(nRow,0,prn_index),
                                            query.value(nameField),
                                            Qt::DisplayRole);
                        data_model->setData(data_model->index(nRow,0,prn_index),
                                            QIcon(":/user.png"),
                                            Qt::DecorationRole);

                        data_model->setData(data_model->index(nRow,1,prn_index),
                                            getMandatFromId(
                                                query.value(s_idFiled).toInt(),
                                                query.value(c_idFiled).toInt()),
                                            Qt::DisplayRole);

                        data_model->setData(data_model->index(nRow,2,prn_index),
                                            query.value(idField),
                                            Qt::DisplayRole);
                        nRow++;
                    }

                }else{
                    dumpError(query.lastError());
                }

            }

        }

    }
}

QString LocalDB::getMandatFromId(int s_id,int c_id)
{
    QString mandat;

    if ( !conState){
        emit sendEventMessageInfo(QObject::trUtf8("Ошибка работы с БД.Нет соединения с локальной базой данных!"),
                                  VPrn::eId_SQL_ExecError,
                                  VPrn::Error );
        return QString();
    }

    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
    {
        QSqlQuery query(db);
        if ( query.exec( QObject::tr("SELECT display_val FROM s_part WHERE id='%1'").arg(s_id,0,10) ) ){
            query.next();
            QString s = query.value( query.record().indexOf("display_val") ).toString();
            mandat.append(s).append(":");
        }else{
            dumpError(query.lastError());
            return QString();
        }
        if ( query.exec( QObject::tr("SELECT display_val FROM c_part WHERE id='%1'").arg(c_id,0,10) ) ){

            query.next();
            QString c = query.value( query.record().indexOf("display_val") ).toString();
            mandat.append( c );
        }else{
            dumpError(query.lastError());
            return QString();
        }
    }
    return mandat;
}

int LocalDB::getUsersCountFromPrinter(int p_id)
{
    if ( !conState){
        emit sendEventMessageInfo(QObject::trUtf8("Ошибка работы с БД.Нет соединения с локальной базой данных!"),
                                  VPrn::eId_SQL_ExecError,
                                  VPrn::Error );
        return -1;
    }
    int users(-1);

    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("MAIN_CON"),true);
    {
        QSqlQuery query(db);
        if ( query.exec(QObject::tr("SELECT count(*) AS cnt FROM rel_prn_usr WHERE prn_id='%1'").arg(p_id,0,10)) ){
            query.next();
            users = query.value( query.record().indexOf("cnt") ).toInt();
        }
    }
    return users;
}

bool LocalDB::createMainDB()
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

bool LocalDB::fillSTable()
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

bool LocalDB::fillCTable()
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
