#ifndef GS_PLUGIN_H
#define GS_PLUGIN_H
#include "igs_plugin.h"
#include "mytypes.h"


#include <QObject>
#include <QtCore/QLibrary>
#include <QtCore/QMap>
#include <QtCore/QStack>

/**
  * @brief Даннай плагин предназначен для осуществления различных преобразований ps и pdf
  * файлов с использованием внешней утилиты pdftk и интерпретатора postscript gs
  * так как каждое преобразование может состоять из множества связанных подзадач
  * а пользователя инетересует конечный результат, то применятся так называемый task pull
  * предсталяющий из себя множество (№задачи,№подзадачи) после завершения очередного шага
  * происходит проверка есть еще подзадачи в рамках этой задачи если нет, то отправляется сообщение
  * о успешном или не успешном решении задачи.
  */
class QStringList;

class GS_plugin :public QObject, Igs_plugin
{
    Q_OBJECT
    Q_INTERFACES(Igs_plugin)
public:
    GS_plugin(QObject *parent=0);
    ~GS_plugin();
    void init(const QString &gs_bin,const QString &pdftk_bin,
              const QString &temp_folder);
    void clear(); 
    QStringList findFilesInDir(const QString &tDir, /* Каталог поиска*/
                               const QStringList &filters /*Тип файлов*/
                               ) const;

signals:
   void pluginReady(const QString &wDir);
   void pluginNotReady();

   void error(VPrn::AppErrorType errCode,QString error_message);
    /**
      * @fn void jobFinish (VPrn::Jobs job_id,int code, const QString &output_message);
      * @param job_id;         Какую работу выполняли
      * @param code;           С каким результатом 0 успешно, 1 и далее ошибка
      * @param output_message; Расширенный ответ
      */
    void jobFinish(VPrn::Jobs job_id,int code,const QString &output_message);
    /**
      * @fn void docConvertedToPdf()
      * @brief Процесс конвертации ps -> pdf завершен
      */
    void docConvertedToPdf();
    /**
      * @fn void docReady4work(qint32 pagecount);
      * @brief Сигнал испускается после завершения конвертации ps->pdf,
      * подсчета числа страниц и разбиения на 1 и последующие страницы
      */
    void docReady4work(qint32 pagecount);
    void docMergedWithTemplate();
    void generatePreViewFinished();

public slots:
       void convertPs2Pdf(const QString &input_fn);
       /**
         * @fn void catFirstPages( );
         * @brief Объединяет первые страницы в один файл.
         */
       void catFirstPages( );
       void mergeWithTemplate();
       void convertPdf2Png(bool mode);

private slots:
       void threadFinish(const QString &jobKey,int code,
                             const QString &output_message);
private:
    QString gsBin;
    QString spoolDir;
    QString pdftkBin;
    QString gs_rcp;
    QString uuid;
    QString mainPdf;
    QString firstPage;
    QString otherPage;
    QString workDir;
    qint32  pCount;
    bool    splitFirst;
    bool    splitOther;

    QStringList myEnv; // Мои переменные среды для gs
    /**
      * @var jobs_list;   Список uuid,job_id
      */
    QMap <QString,VPrn::Jobs> jobs_list;

    QHash <QString,int> metaJob_counter;

    /**
      * @fn void recursiveDeletion(QString path);
      * @brief � екурсивное удалаление каталога со всем его содержимым
      */
    void recursiveDeletion(const QString &path);
    void createRCPfile();
      /**
      * @fn void start_proc(const QString &bin,
                            QStringList &arg_list,VPrn::Jobs job_id);
      * @brief Запускает отдельный поток обработки задания
      */
    void start_proc(const QString &bin,QStringList &arg_list,VPrn::Jobs job_id);
        /**
      * @fn  void calcPageCount(const QString &input_fn);
      * @brief Используя pdfTk получим число страниц в pdf документе
      */
    void calcPageCount(const QString &input_fn = QString());
    qint32 getPageCountFromProcOut(const QString &out_str);
        /**
      * @fn void splitPdf();
      * @brief Делит файл имеющий на блоки первая страница, остальные страницы
      */
    void splitPdf();
    void checkSplitState();

    void catPdf(const QString &file_nameA,const QString &file_nameB,
                const QString &output_name);
    void mergePdf(const QString &in_pdf,
                             const QString &back_pdf,const QString &out_pdf);



    void pdf2png(const QString &in_pdf,const QString &out_pdf,bool mode);
    void incJobsCount(const QString &job_id);
    bool isJobFinished(const QString &job_id);
};


#endif

