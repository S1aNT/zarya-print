#ifndef DB_GATEWAY_H
#define DB_GATEWAY_H

#include <QObject>
#include "message.h"
#include "mytypes.h"

#include <QtSql/QSqlDatabase>

using namespace VPrn;

QT_FORWARD_DECLARE_CLASS ( QSqlQuery )
        QT_FORWARD_DECLARE_CLASS ( QSqlQueryModel )
        QT_FORWARD_DECLARE_CLASS ( QSqlError )
        QT_FORWARD_DECLARE_CLASS ( QSqlTableModel )

        class Db_GateWay : public QObject
{
    Q_OBJECT
public:
    explicit Db_GateWay(QObject *parent = 0);
    ~Db_GateWay();
    bool openMainBase(const QString &base_file);


signals:
    /**
      * @fn void informationToClient(const QString &c_uuid,const QString &info);
      * @brief Отравляется когда сервер хочет отправить клиенту информационное сообщение
      */
    void informationToClient(const QString &c_uuid,const QString &info);

    void setDataBaseSlice (const QString &m_c_uuid, const QByteArray &m_data);

    void userListSaved(const QString &c_uuid,bool state);
    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);

public slots:
    /**
      * @fn void saveUserToBase(QStringList & u_list);
      * @brief сохраняет список пользователей в БД актуализируя информацию.
      * u_list -> временная таблица input -> основаная таблица users
      * Данные из временной таблицы input загружаются в основную таблицу
      * в несколько этапов.
      * @li 0. Чистим таблицу input
      * @li 1. Удаляем из таблицы связей rel_prn_usr все записи usr_id
      * которых принадлежит тем пользователям который удалены.
      * т.е их нет переданом списке u_list и следовательно нет в таблице input
      * @li 2. Удаляем из таблицы users тех пользователей которых НЕТ в списке u_list
      * @li 3. Добавлем в таблицу users тех пользователей которых ЕСТЬ в списке u_list
      * при этом проверяем что бы небыло дуликатов. т.е добавляем только НОВЫХ пользователей

      */
    void saveUserToBase(const QString &c_uuid,QStringList & u_list);

    void closeMainDb();

    /**
      * @fn void getDataBaseSlice(const QString &c_uuid,const QString &login,const QString &mandat);
      * @param const QString &c_uuid - индентификатор клиента который инициировал запрос
      * @param const QString &login  - логин пользователя
      * @param const QString &mandat - мандат пользователя
      * @return Возвращает сигнал содержащий срез базы Принтеры, Пользователи, Расширенные атрибуты принтеров
      * и самое главное СИНХРО - индентификатор последнего изменения базы для синхронизации базы при обратном ответе
      * от клиента. Синхро - это отдельная таблица из 3-х полей PRN,USR,ATR каждое поле содержит номер время последней модификации
      * соответствующей таблицы
      */
    void getDataBaseSlice(const QString &c_uuid,const QString &login,const QString &mandat);


private:
    bool m_DriverValid;
    /**
      * @fn int getUserId(QSqlDatabase db,const QString &login, int s_id, int c_id);
      * @param QSqlDatabase db  подключение к рабочей БД
      * @param const QString &login логин пользователя переданный клиентом
      * @param int s_id идентификатор S части мандата
      * @param int c_id идентификатор C части мандата
      * @return int Возвращает id пользователя или -1 если такого нет
      * @brief Ищет в таблице пользователей  юзера отвечающего запрошенным атрибутам
      */
    int getUserId(QSqlDatabase db,const QString &login, int s_id, int c_id);
    int getPartTableId(QSqlDatabase db,const QString &table, const QString &value);

    bool createMainDB();


    bool fillSTable();
    bool fillCTable();

    bool initDB(QSqlDatabase db);
    void dumpError (const QSqlError & lastError);
    /**
      * @fn int compare(const QString &M1, const QString & M2);
      * @param const QString &M1 - мандат в виде S[015]:C[0-1024]
      * @param const QString &M2 - мандат в виде S[015]:C[0-1024]
      * @return -1,0,1 (M1 меньше,равно,больше M2)

      * @brief Функция сравнивает два мандата между собой.
      * Если S1 < S2  то -1
      * Если S1 > S2  то  1
      * Если S1 = S2  и С1 < С2 то -1
      * Если S1 = S2  и С1 = С2 то 0
      * Если S1 = S2  и С1 > С2 то 1
      */
    int compare(const QString &M1, const QString & M2);

    QString m_dbFile;

};

#endif // DB_GATEWAY_H
