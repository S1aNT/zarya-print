#include <QtCore/QCoreApplication>
#include "pwdparser.h"
#include <iostream>

void print_help()
{
    std::cout << "Usage: users_update [--help] [--server=<IP or Hostname>] [--port=<Port>] --start_pid=<PID> " << std::endl;
    std::cout << std::endl;
    std::cout << "Example: users_update --server=192.168.112.2 --port=4243 --start_pid=499" << std::endl;

}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "Reader users from /etc/passwd and sending ControlServer." << std::endl;
    std::cout << "Created by Sl@NT <p.slant@gmail.com>" << std::endl;
    std::cout << std::endl;

    QString serverName=QString();
    int serverPort=0;
    int startPid=0;


    foreach( QString ar, a.arguments()) {
        if ( ar.split('=').size() == 2 ) {
            QString key   = ar.split('=').at(0);
            QString value = ar.split('=').at(1);

            if (key.compare("--server",Qt::CaseInsensitive)==0){
                serverName = value;
            }
            if (key.compare("--port",Qt::CaseInsensitive)==0){
                serverPort = value.toInt();

            }
            if (key.compare("--start_pid",Qt::CaseInsensitive)==0){
                startPid = value.toInt();
            }
        }
        else {
            if ( ar == "--help" ){
                print_help();
                return 0;
            }
        }
    }
    if (!serverName.isEmpty() && serverPort >0 && startPid> 0){
        PwdParser pwdParser;
        return pwdParser.findAndSendUser(serverName, serverPort,startPid);
    }else{
        print_help();
        return -1;
    }



    //return a.exec();
}
