#ifndef INTROPAGE_H
#define INTROPAGE_H

#include <QWidget>
#include "mytypes.h"

using namespace VPrn;

class QCheckBox;

class IntroPage : public QWidget
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent=0);
signals:
    void enableNext();

public slots:
    void setStep(VPrn::InitSteps step);

private:
    QCheckBox *inputFile_step;
    QCheckBox *getAuthData_step;
    QCheckBox *loadPlugin_step;
    QCheckBox *connect2RemoteDemon_step;
    QCheckBox *authPassed_step;
    QCheckBox *reciveSecLabel_step;
    QCheckBox *recivePrintersList_step;

    void checkSteps();

};

#endif // INTROPAGE_H
