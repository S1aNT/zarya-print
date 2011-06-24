#ifndef ERRORLOGGER_H
#define ERRORLOGGER_H

#include <QObject>
#include "qtservice.h"
#include "mytypes.h"
using namespace VPrn;

class ErrorLogger : public QObject
{
    Q_OBJECT
public:
    ErrorLogger(QObject *parent=0);
public slots:
    void saveEventMessageInfo(QString eMsg,
                          VPrn::EventLogMessageId eventCode     = VPrn::eId_EmptyMessage,                          
                          VPrn::EventLogType      evenType      = VPrn::Success,
                          VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);

};

#endif // ERRORLOGGER_H
