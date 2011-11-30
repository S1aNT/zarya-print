#include <QDebug>
#include "elogger.h"

#if defined(Q_OS_UNIX)
#include "syslog.h"
#endif

ELogger::ELogger(QObject *parent)
    : QObject(parent)
{

}

void ELogger::saveEventMessageInfo(QString eMsg,
                                   VPrn::EventLogMessageId eventCode,
                                   VPrn::EventLogType      evenType,
                                   VPrn::EventLogCategory  eventCategory)
{
#if defined(Q_OS_UNIX)
    int st;

    switch(evenType) {
    case VPrn::Error:
        st = LOG_ERR;
        break;
    case VPrn::Warning:
        st = LOG_WARNING;
        break;
    default:
        st = LOG_INFO;
    }


    openlog("CLIENT_PRN", LOG_PID, LOG_DAEMON);

    foreach(QString line, eMsg.split('\n'))
        syslog(st, "%s", line.toLocal8Bit().constData());
    closelog();

#endif
    qDebug() << "\neMsg: "    << eMsg
             << "\nevenType:" << evenType
             << "\neventCode" << eventCode
             << "\neventCategory:"<< eventCategory
             << "\n----------------------------------------------------------\n";

}
