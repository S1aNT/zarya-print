#ifndef AUTHPLUGIN_H
#define AUTHPLUGIN_H

#include <stdio.h>

#include "iauth_plugin.h"
#include "mytypes.h"

#define DEBUG

using namespace VPrn;


class AuthPlugin : public QObject, IAuth_plugin
{
    Q_OBJECT
    Q_INTERFACES(IAuth_plugin)

public:
    AuthPlugin(QObject*parent = 0);

    void init();

signals:
    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);

    void userAuth(const QString &user,const QString &role,const QString &mls,const QString &mcs);

private:
    QString user;
    QString role;
    QString mlsRange;
    QString mcsRange;
};

#endif
