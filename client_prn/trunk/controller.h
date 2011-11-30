#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QStringListModel>
#include <QStandardItemModel>

#include "mytypes.h"
#include "igs_plugin.h"
#include "iauth_plugin.h"
#include "networkclient.h"
#include "templatesdatagateway.h"
#include "message.h"


#ifdef D_MYDEBUG
#include "fakenetans.h"
#endif

using namespace VPrn;
class Controller : public QObject
{
    Q_OBJECT
public:
    Controller(QObject*parent = 0);
    ~Controller();
    bool init(const QString &app_dir,const QString &wFile);

    /// @todo Нужен отдельный класс хранилище данных там всем этим моделям и место
    QStringListModel   *getStampListModel(){return stampListModel;}
    QStringListModel   *getPictureListModel(){return pictureListModel;}
    QStandardItemModel *getPrinterModel(){return printerModel;}
    QStandardItemModel *getCardModel(){return cardModel;}
    QStandardItemModel *getTemplatesModel(){return templatesModel;}
    /**
      * @fn void clearPreViewFiles();
      * @brief Очистка каталогов с экземплярами документа
      * удаляем сгенерированный шаблон+ документ с наложенным шаблоном + файла предпросмотра
      */
    void clearPreViewFiles();
signals:
    void finishStep(VPrn::InitSteps step);
    void sendEventMessageInfo(QString eMsg,
                              VPrn::EventLogMessageId eventCode,
                              VPrn::EventLogType      evenType,
                              VPrn::EventLogCategory  eventCategory = VPrn::eCatId_Empty);
    void needUpdatePicturesList();

    void serverInfo(QString,QString);
    void setCardValid(bool);

public slots:
    void beginPrintCurrentDoc();
    void genFullPreView();
    void genPartPreView();

private slots:
    void saveAuth(const QString &user,const QString &role,
                  const QString &mlsRange,const QString &mcsRange);
    void parseNetworkMessage(const Message & msg);
    void doConnect();
    void gsPluginReady(const QString &wDir);
    void docReady4work(qint32 pCount);
    void docConvertedToPdf();    
    void docMergedWithTemplate();
    void generatePreViewFinished();

    /**
      * @fn void itemChangedCardsModel(QStandardItem* item);
      * @brief При изменении элемента ячейки происходит проверка, что
      * колонка модели  ==  id гриф секретности
      * в ячеку записывается текстовое название грифа секретности
      */
    void itemChangedCardsModel(QStandardItem* item);

private:
    Igs_plugin   *myGs_plugin;
    IAuth_plugin *myAuth_plugin;


#ifdef D_MYDEBUG
    FakeNetAns * m_netClient;
#else
    NetWorkClient * m_netClient;
#endif
    TemplatesDataGateway *tDataGW;

    QString serverName;
    qint32 serverPort;
    QString gsBin;
    QString pdftkBin;
    QString spoolDir;
    QString localTDir;
    QString globalTDir;
    QString workFile;
    QString m_user;
    QString m_role;
    QString m_mls;
    QString m_mcs;
    QStringListModel   *stampListModel;
    QStandardItemModel *printerModel;
    QStandardItemModel *cardModel;
    QStandardItemModel *templatesModel;
    QStringListModel   *pictureListModel;
    QStringList templatesList;

    QString currentPrinter;
    QString currentStamp;
    QString currentTemplate;
    QString workingDir;
    int preViewMode;

    bool readConfig(const QString &app_dir);
    bool loadPlugins();
    bool isSavePrinterToModel (QByteArray data);
    void findTemplates(const QString &t_path);   
};

#endif // CONTROLLER_H
