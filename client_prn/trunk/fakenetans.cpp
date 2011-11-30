#include "fakenetans.h"
#include <QTimer>
#include <QByteArray>
#include <QRegExp>
#include <QFile>
#include <QTimer>
#include <QDataStream>
#include <QUuid>
#include <QDate>

FakeNetAns::FakeNetAns(QObject *parent)
    : QObject(parent)
{


}

void FakeNetAns::connectToHost(const QString &serverName,qint32 serverPort)
{
    QTimer::singleShot(1000,this, SIGNAL( connectToRemoteServer() ));
}

//------------------------ Publis Slots ----------------------------------------

void FakeNetAns::reSendNetworkMessage(const Message &r_msg)
{
    QString s_data;


    s_data.append(r_msg.getMessageData());

    switch (r_msg.getType()){
    case VPrn::Que_AuthUser:
        if (this->checkAuthData() ){
            this->getSecretLevel();
            this->getPrinterList();
        }
        break;
    case VPrn::Que_CheckDocInBase:
        this->checkDocInBase(r_msg.getMessageData());
        break;
    default:
        break;
    }


}

//------------------------ Private slots ---------------------------------------



//------------------------ Private ---------------------------------------------

bool FakeNetAns::checkAuthData()
{
    Message s_msg( this ); // Формируем ответ
    s_msg.setType(VPrn::Ans_AuthPassed);
    emit reciveMessage(s_msg);
    return true;
}

void FakeNetAns::getSecretLevel()
{
    Message s_msg( this ); // Формируем ответ
    s_msg.setType(VPrn::Ans_StampList);
    QString body = QObject::trUtf8("Сов.секретно;:;Секретно;:;Не секретно");
    s_msg.setMessageData( body.toUtf8() );
    emit reciveMessage(s_msg);
}

void FakeNetAns::getPrinterList()
{
    Message s_msg( this );
    s_msg.setType(VPrn::Ans_PrinterList);
    QByteArray body;
    QDataStream out (&body,QIODevice::WriteOnly);
    out << (qint32)10; //Printer count
    for (int i=0; i<10; i++){
        out << QUuid::createUuid().toString().replace("{","").replace("}",""); // printer id
        out << QObject::trUtf8("hostname.Printer_%1").arg(i,0,10); // printer name
    }
    s_msg.setMessageData( body );

    emit reciveMessage(s_msg);
}

/**
  * @brief Метод проверят возможность записи в БД карточки документа.
  * Это возможно при выполнении следующих условий:
  * @li МБ не существует
  * @li МБ существует, но номер экз. (1-5) не существует
  * @li МБ существует, но номер экз. (1-5) существует, название док-та совпадает,
  * и документ помечен как БРАК или отсутвует пометка напечатан.
  * если запись карточки документа не возможна, то пользователю отправляется сообщение
  * Невозможно сохранить карточку документа [ПРИЧИНА ОТКАЗА]
  */

void FakeNetAns::checkDocInBase(QByteArray data)
{
    QDataStream in (&data,QIODevice::ReadOnly);
    QString mb;
    QString docName;
    qint8 copyNumber;

    in >> docName;
    in >> mb;
    in >> copyNumber;

    Message s_msg( this );

    switch( this->findInBase(docName,mb,copyNumber) ){
    case 0:
    case 1:
    case 2:
         s_msg.setType(VPrn::Ans_CheckDocInBase);
        break;
    case 3:
        s_msg.setType(VPrn::Info_Message);
        QByteArray body;
        QDataStream out (&body,QIODevice::WriteOnly);
        out << QObject::trUtf8("Невозможно сохранить карточку документа.");
        out << QObject::trUtf8("В базе данных уже существует документ с названием [%1]\n"
                               "номер МБ -[%2], номер экз. [%3]\n"
                               "Зарегистррован [%4] пользователем [user_007]."
                               "Если Вам необходимо перепечатать данный документ,то обратитесь к администратору безопасности \n"
                               "для изменения статуса документа."
                               ).arg(docName).arg(mb).arg(copyNumber,0,10)
                                .arg(QDate::currentDate().toString());

        s_msg.setMessageData( body );
        break;
    }
     emit reciveMessage(s_msg);
}

qint8 FakeNetAns::findInBase(const QString &doc_name, const QString &mb,qint8 copy_number)
{
    return qrand() % 3;
}
