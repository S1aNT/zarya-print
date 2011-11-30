#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>

#include "mytypes.h"
#include "message.h"
#include "networkclient.h"

using namespace VPrn;

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QObject *parent = 0);
    ~Engine();



    void setServer(const QString &host=QString(),qint16 port=0);

signals:
    void connected();
    void authUserPass();
    void error(VPrn::AppErrorType errCode,QString error_message);
    void definedAuthData(const QString &login, const QString & mandat);
    void saveBaseSlice(QByteArray *data);

public slots:


    void getUserId();
    void parseNetworkMessage(const Message & msg);

private slots:

    /**
      * @fn void authUser();
      * @brief Формируем запрос к УС в ответе получим данные :
      * @li Список принтеров в системе защищенной печати
      * @li Список пользователей в системе защищенной печати
      * @li Расширенные атрибуты каждого принтера (Метка секретности, список допущенных пользователей)
      * Данные сохраняются в локальной БД приложения все изменения отправляются на сервер
      * Ограничения. Список принтеров будет ограничен мандатом пользователя, не выше чем его мандат
     */

    void authUser();

private:


    NetWorkClient *m_netClient;



    QString m_login;
    QString m_mandat; // S0-15:C0-1024

};

#endif // ENGINE_H
