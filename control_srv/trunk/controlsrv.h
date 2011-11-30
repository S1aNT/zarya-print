/*
 * @class ControlSrv
 * @brief ControlSrv is the class that implements  simple tcp|ip server listen port 4243 and wait connection.
 *This is server work's with clients, such as tEditor, printMonitor and Reporter
 */

#include "localserver.h"
#include "msgdispatcher.h"
#include "db_gateway.h"

#include "errorlogger.h"

class ControlSrv : public QtService<QCoreApplication>
{
public:
    ControlSrv(int argc, char **argv);
protected:
    void start();
    void pause();
    void resume();    


private:
    ErrorLogger *eLogger;
    LocalServer *locServer;
    MsgDispatcher *msgDispatcher;
    Db_GateWay * db_gateway;
    QString HostName;
    quint16 Port;
    QString mainDB;

    bool readConfig();
};
