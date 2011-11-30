#ifndef TEMPLATESDATAGATEWAY_H
#define TEMPLATESDATAGATEWAY_H

#include <QStandardItemModel>

#include "mytypes.h"
#include <QtSql/QSqlDatabase>

using namespace VPrn;

QT_FORWARD_DECLARE_CLASS ( QSqlQuery )
QT_FORWARD_DECLARE_CLASS ( QSqlQueryModel )
QT_FORWARD_DECLARE_CLASS ( QSqlError )
QT_FORWARD_DECLARE_CLASS ( QSqlQueryModel )



class TemplatesDataGateway : public QObject
{
    Q_OBJECT
public:
    explicit TemplatesDataGateway(QObject *parent = 0);
    ~TemplatesDataGateway();

    void setWorkingDir(const QString &wDir);

    void setTmplModel(QStandardItemModel *m){t_model=m;}
    void setCardModel(QStandardItemModel *m){card_model=m;}

    void getMetaInfo(const QStringList &list);
/**
      * @brief Преобразует выбранный шаболон tName в набор pdf расположенный в подкаталогах
      * относительно базового wDir. Подкаталоги по номерам копий 1-copy... 5-copy
      */

    void prepareTemplate(const QString &t_fileName);
signals:
    void finishGenTemplate();
    void sendEventMessageInfo(QString eMsg,
                                  VPrn::EventLogMessageId eventCode,
                                  VPrn::EventLogType      evenType,
                                  VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);

public slots:



private:    
    QStandardItemModel *t_model;
    QStandardItemModel *card_model;
    QSqlQueryModel *pages_model;

    QString workDir;
    bool sqlDrvLoad;


    void readMeta(const QString &fileName);
    void dumpError (const QSqlError & lastError);
    void getPageInfoFromModel(const QString &fileName,int &p_angle,qreal &p_width, qreal &p_height);
    bool saveModelToBase();
    QVariant getDataFromCell(int row,int col);


    bool updateElementsTable(const QString &m_tag, const QString &m_value);

    bool isCopyValid(int pCopy);
    void printPageToPdf(int pages_number,int angle,qreal width,qreal height,const QString &fileName);

};

#endif // TEMPLATESINFO_H
