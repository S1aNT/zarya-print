#include <QDebug>
#include "pwdparser.h"
#include <QFile>
#include <QTextStream>

PwdParser::PwdParser(QObject *parent)
        :   QObject(parent)
{


}


void  PwdParser::startFindPrinters()
{
    qDebug() << Q_FUNC_INFO;

}

