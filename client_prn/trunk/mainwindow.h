#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QtGui/QDesktopWidget>
#include "mytypes.h"
#include "controller.h"
#include "intropage.h"
#include "editcardpage.h"
#include "previewpage.h"

#include "elogger.h"
#include "selectway.h"

class QErrorMessage;
class QPushButton;
class QStackedWidget;

using namespace VPrn;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init(const QString &app_dir,const QString &wFile);
public slots:
    void appendStartMsg(const QString &txt_msg);
private slots:
    void help();
    void do_next();
    void do_prev();
    void do_restart();
    void enableNext();
    void showInfo(const QString &title,const QString &txt);

#ifdef D_MYDEBUG
    void fakePrint();
#endif
private:

    QDesktopWidget desktop;
    QRect desktop_avail;

    QErrorMessage  *myEMsgBox;

    QPushButton *helpButton;
    QPushButton *prevButton;
    QPushButton *nextButton;
    QPushButton *restartButton;
    QPushButton *exitButton;

    Controller     *controller;
    ELogger        *eLogger;
    IntroPage      *introPage;
    EditCardPage   *editCardPage;
    PreViewPage    *preViewPage;
    QStackedWidget *stackedWidget;


    QPoint calcDeskTopCenter(int width,int height);

    void connectAll();

};

#endif // MAINWINDOW_H
