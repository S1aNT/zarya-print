#define NO_DEMON_MODE_OFF
#include <QDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QTextCodec>

#if !defined(Q_WS_WIN)
#include <QtCore/QDir>
#include <QtCore/QSettings>
#endif

#include "controlsrv.h"



int main(int argc, char **argv)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
#if defined(Q_WS_WIN)
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("CP866"));
#else
    QTextCodec::setCodecForLocale(codec);
#endif

/*
#if !defined(Q_WS_WIN)
    // QtService stores service settings in SystemScope, which normally require root privileges.
    // To allow testing this example as non-root, we change the directory of the SystemScope settings file.
    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QDir::tempPath());
    qWarning("(Example uses dummy settings file: %s/QtSoftware.conf)", QDir::tempPath().toLatin1().constData());
#endif
*/

    ControlSrv daemon(argc, argv);
    return daemon.exec();

//#endif

}
