#ifndef IAUTH_PLUGIN_H
#define IAUTH_PLUGIN_H
#include "mytypes.h"

using namespace VPrn;

QT_FORWARD_DECLARE_CLASS(QString)

class IAuth_plugin{

public:
    virtual ~IAuth_plugin() {}
    virtual void init()=0;
    virtual void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty)=0;

    virtual void userAuth(const QString &user,const QString &role,const QString &mls,const QString &mcs)=0;

};


QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(IAuth_plugin,"com.Zarya.Plugin.IAuth_plugin/2.0");
QT_END_NAMESPACE


#endif
