#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QSet>
#include <QMap>

#include "qtservice.h"
#include "message.h"
#include "mytypes.h"

using namespace VPrn;

class LocalServer : public QTcpServer
{
    Q_OBJECT
public:
    LocalServer( QObject* parent = 0);
    void pause();
    void resume();
signals:    

    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);

    void reciveMessage(const QString &c_uuid,const Message &msg);
public slots:
    void sendMessage  (const QString &c_uuid,const Message &msg);

private slots:
    void client_init();

    void readyRead();
    void disconnected();
    void prepareError(QAbstractSocket::SocketError socketError);

private:
    /**
      * @var packetSize; Размер передаваемого блока данных
      */
    bool disabled;
    PACKET_SIZE packetSize;

    QSet<QTcpSocket *> clients;
    QMap<QTcpSocket *,QString> clients_uuid;    


    QString getUuid() const;
    QTcpSocket *findClient(const QString &c_uuid);
};



