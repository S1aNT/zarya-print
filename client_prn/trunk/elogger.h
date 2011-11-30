#ifndef ELOGGER_H
#define ELOGGER_H

#include <QObject>
#include "mytypes.h"

using namespace VPrn;

class ELogger : public QObject
{
    Q_OBJECT
public:
    explicit ELogger(QObject *parent = 0);

public slots:
    void saveEventMessageInfo(QString eMsg,
                             VPrn::EventLogMessageId eventCode     = VPrn::eId_EmptyMessage,
                             VPrn::EventLogType      evenType      = VPrn::Success,
                             VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);

};

#endif // ELOGGER_H
