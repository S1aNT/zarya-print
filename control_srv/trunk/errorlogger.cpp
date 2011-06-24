#include "errorlogger.h"
#include <QDebug>
ErrorLogger::ErrorLogger(QObject *parent)
        :QObject(parent)
{

}


void ErrorLogger::saveEventMessageInfo(QString eMsg,
                                       VPrn::EventLogMessageId eventCode,
                                       VPrn::EventLogType      evenType,
                                       VPrn::EventLogCategory  eventCategory)
{
    QtServiceBase::instance()->logMessage(eMsg,
                                          (QtServiceBase::MessageType)evenType,
                                          (int) eventCode,(uint)eventCategory);
    qDebug() << "\neMsg: "    << eMsg
             << "\nevenType:" << evenType
             << "\neventCode" << eventCode
             << "\neventCategory:"<< eventCategory
             << "\n----------------------------------------------------------\n";
}

