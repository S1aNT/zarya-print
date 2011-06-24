#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QStandardItemModel>
#include "mytypes.h"

using namespace VPrn;

class Engine : public QObject
    {
    Q_OBJECT
public:
    explicit Engine(QObject *parent = 0);
    ~Engine();

    QStandardItemModel *model(){return data_model;}

signals:
    void error(VPrn::AppErrorType errCode,QString error_message);

private:
    QStandardItemModel *data_model;
    };

#endif // ENGINE_H
