#include <QtSingleApplication>
#include <QtGui/QMessageBox>
#include <QtCore/QTextCodec>
#include <QtCore/QStringList>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "mainwindow.h"

#include "config.h"

int main(int argc, char *argv[])
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
    QtSingleApplication app(argc, argv, true);

    //installLog("SURD_GUI",QObject::trUtf8("Заря"));

    if (app.sendMessage(QObject::trUtf8("Потребован повторный запуск приложения!"))
        || app.isRunning())
        return 0;

    QApplication::setQuitOnLastWindowClosed(true);

    qApp->setStyleSheet(" QPushButton:hover {background-color: black;} "
                        " QPushButton:hover {background-color: white;} "
                        " QPushButton { border-style: outset; border-width: 2px;"
                        "  border-radius: 10px; border-color: beige;"
                        "  font: bold 10px;  min-width: 10em; padding: 6px}"
                        );

    MainWindow client;   

    client.show();
    client.init ( qApp->applicationDirPath() );

    app.setActivationWindow(&client);

    QObject::connect(&app, SIGNAL(messageReceived(const QString&)),
                     &client, SLOT(appendStartMsg(const QString&)));

    return app.exec();
}
