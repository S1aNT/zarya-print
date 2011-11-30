#ifndef LOCALDB_H
#define LOCALDB_H

#include "mytypes.h"
#include "message.h"
#include "networkclient.h"
#include <QSqlTableModel>
#include <QStandardItemModel>

using namespace VPrn;

class LocalDB : public QObject
{
    Q_OBJECT
public:
    LocalDB(QObject *parent = 0);
    ~LocalDB();

    QStandardItemModel *getDatamodel(){return data_model;}
public slots:
    /**
      * @brief Полученный от управляющего сервера дамп БД востанавливает во временную БД в памяти
      */
    void restoreDump(QByteArray *data);
    QString getPrinterNameFromIndex(QModelIndex *index);
    QString getPrinterMandatFromIndex(QModelIndex *index);
signals:
    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);


private:
    QSqlTableModel *prnModel;
    QSqlTableModel *usrModel;
    QSqlTableModel *relModel;
    QStandardItemModel *data_model;

    bool conState;

    bool createConnection();
    void dumpError (const QSqlError & lastError);
    void setData();

    /**
      * @fn QString getMandatFromId(int s_id,int c_id) const;
      * @param int s_id
      * @param int c_id
      * @return QString Текстовое представление мандата S0:C0
      */
    QString getMandatFromId(int s_id,int c_id);
    /**
      * @fn int getUsersCountFromPrinter(int p_id);
      * @param int p_id id Принтера
      * @return int число пользователей допущенных к данному принтеру
      */
    int getUsersCountFromPrinter(int p_id);

    bool createMainDB();
    bool fillSTable();
    bool fillCTable();
};

#endif // LOCALDB_H
