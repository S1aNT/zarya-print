#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QStandardItemModel>
#include <QMap>

#include "mytypes.h"
#include "message.h"

using namespace VPrn;

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QObject *parent = 0);
    ~Engine();

    QStandardItemModel *model(){return data_model;}
    QStandardItemModel *prnModel(){return prnInfoModel;}
public slots:
    void getUserId();
    void parseNetworkMessage(const Message & msg);
    /**
      * @brief Заполняет модель Юзер!Мандат!ID для принтера с ID = prnID
      * и добавляет туда список всех пользователей
      */
    void getUsersListForPrinter(qint32 prnId);

signals:
    void error(VPrn::AppErrorType errCode,QString error_message);
    void definedAuthData(const QString &login, const QString & mandat);
private:
    /**
      * @fn void updateData();
      * @brief Обновляет данные в моделях СПИСОК_ПРИНТЕРОВ, СПИСОК_ПОЛЬЗОВАТЕЛЕЙ
      */
    void updateData();

    QStandardItemModel *data_model;
    QStandardItemModel *prnInfoModel;
    QString m_login;
    QString m_mandat; // S0-15:C0-1024

    //PrinterInfoList prnList;
    //UserInfoList    userList;
    //QMap<qint32,qint32> relPrnUsr; // Таблица связей Принтер->Допущенные поьзователи
};

#endif // ENGINE_H
