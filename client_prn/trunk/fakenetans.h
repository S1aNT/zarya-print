#ifndef FAKENETANS_H
#define FAKENETANS_H

#include <QObject>
#include "mytypes.h"
#include "message.h"

using namespace VPrn;

class FakeNetAns : public QObject
{
    Q_OBJECT
public:
    explicit FakeNetAns(QObject *parent = 0);
    void connectToHost(const QString &serverName,qint32 serverPort);
signals:
    void connectToRemoteServer();
    void reciveMessage(const Message &r_msg);
public slots:
    void reSendNetworkMessage(const Message &r_msg);


private:
    bool checkAuthData();
    void getSecretLevel();
    void getPrinterList();
    void checkDocInBase( QByteArray data);
    qint8 findInBase(const QString &doc_name, const QString &mb,qint8 copy_number);
};

#endif // FAKENETANS_H
