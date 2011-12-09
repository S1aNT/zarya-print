#ifndef PWDPARSER_H
#define PWDPARSER_H

#include <QObject>

#include "mytypes.h"
#include "netclient.h"

using namespace VPrn;

class PwdParser : public QObject
{
    Q_OBJECT
public:
    explicit PwdParser(QObject *parent = 0);




signals:
    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);
    void foundPrinters(QStringList &u_list);
public slots:
    void startFindPrinters();

private:

};

#endif // PWDPARSER_H
