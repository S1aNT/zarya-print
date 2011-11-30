#include "gs_plugin.h"
#include "proc_thread.h"

#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFileInfoList>
#include <QtCore/QRegExp>
#include <QtCore/QTemporaryFile>
#include <QtCore/QTextStream>
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QHash>
#include <QMutableHashIterator>

//#include <QtGui/QPixmap>

GS_plugin::GS_plugin(QObject *parent)
    :QObject (parent)
{
    uuid = QUuid::createUuid().toString().replace("{","").replace("}","");   
}

GS_plugin::~GS_plugin()
{
}

void GS_plugin::clear()
{
    QFile::remove(gs_rcp);
    this->recursiveDeletion(workDir);

}


void GS_plugin::init(const QString &gs_bin, const QString &pdftk_bin,
                     const QString &temp_folder)
{

    spoolDir = temp_folder;
    pdftkBin = pdftk_bin;
    gsBin = gs_bin;
    myEnv << QObject::trUtf8("TMPDIR=%1").arg(spoolDir);
    workDir = QObject::trUtf8("%1/%2").arg( spoolDir, uuid ) ;
    QDir wDir;
    wDir.mkpath(workDir);
    // Формируем основной выходной файл
    mainPdf   = QObject::trUtf8("%1/main.pdf").arg( workDir ) ;
    firstPage = QObject::trUtf8("%1/firstpage.pdf").arg( workDir );
    otherPage = QObject::trUtf8("%1/otherpage.pdf").arg( workDir );

    createRCPfile();
}

QStringList GS_plugin::findFilesInDir(const QString &tDir,
                                      const QStringList &filters ) const
{
    QStringList files;
    QDir dir(tDir);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setNameFilters(filters);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i){
        QFileInfo fileInfo = list.at(i);
        files.append(dir.absoluteFilePath(fileInfo.fileName()));
    }

    return files;
}
//------------------------------------------ Public slots -------------------------------------------------
void GS_plugin::convertPs2Pdf(const QString &input_fn)
{
    // Проверим фактик наличия входного файла
    qDebug() << Q_FUNC_INFO << input_fn;
    if (QFile::exists(input_fn)) {
        // Формируем строку команды для первого этапа
        QStringList args;
        args.append("-q" );
        args.append(QString("@%1").arg(gs_rcp));
        args.append(QString("-sOutputFile=%1").arg( mainPdf ) );
        args.append("-c");
        args.append(".setpdfwrite");
        args.append("-f");
        args.append(input_fn);
        start_proc(gsBin,args,VPrn::job_ConvertPs2Pdf);
    }else {
        emit error(VPrn::FileIOError,
                   QObject::trUtf8("ERROR : Файл %1 не найден\n%2")
                   .arg(input_fn)
                   .arg(QString(Q_FUNC_INFO)));
    }

}

void GS_plugin::catFirstPages()
{

}

void GS_plugin::mergeWithTemplate()
{
    QString copy_dir;
    /**
      * Делаю неявную блокировку. для исключения ситуации когда один из потоков
      * merge завершиться самым первым до старта других потоков и начнется процесс получения картинок
      * для незаконченных потоков
      * @todo переписать с помощью мьтекса
      */
    this->incJobsCount("merge_pdf");
    for (int p_copy=VPrn::FirstPage; p_copy <= VPrn::FirstPageN5;p_copy++){
        copy_dir = QObject::trUtf8("%1/%2-copy").arg(workDir).arg(p_copy,0,10);
        QStringList t_files = findFilesInDir(copy_dir,QStringList()<<"t_*.pdf");
        QString backPdf;
        QString outPdf;
        for (int i=0; i< t_files.size();i++){
            backPdf = t_files.at(i);
            // spoolDir/client_uuid/t_firstpage.pdf
            QRegExp rx("/(.+)/(.+)/(.-copy)/(t_.+).pdf");
            if(rx.indexIn( backPdf ) != -1){
                // Наш файлик можно обрабатывать
                QString copy_num = rx.cap(3);
                QString page_type  = rx.cap(4);

                if ( page_type.compare("t_firstpage",Qt::CaseInsensitive) == 0 ){
                    // Лицевая сторона i экз
                    outPdf = QString("%1/%2/firstpage_out.pdf")
                            .arg(workDir,copy_num);
                    mergePdf(firstPage,backPdf,outPdf);
                }
                if ( page_type.compare("t_otherpage",Qt::CaseInsensitive) == 0 ){

                    outPdf = QString("%1/%2/otherpage_out.pdf")
                            .arg(workDir,copy_num);

                    if ( QFile::exists(otherPage) ){
                        // Лицевая сторона второго и последующих листов
                        mergePdf(otherPage,backPdf,outPdf);
                    }
                }
                if ( page_type.compare("t_oversidepage",Qt::CaseInsensitive) == 0){
                    outPdf = QString("%1/%2/oversidepage_out.pdf")
                            .arg(workDir,copy_num);
                    QFile::copy(backPdf,outPdf);
                }
                if ( page_type.compare("t_lastpage",Qt::CaseInsensitive) == 0){
                    outPdf = QString("%1/%2/lastpage_out.pdf")
                            .arg(workDir,copy_num);
                    QFile::copy(backPdf,outPdf);
                }

            }
        }
    }
    this->isJobFinished("merge_pdf");
}

void GS_plugin::convertPdf2Png(bool mode)
{
    QString copy_dir;
    /**
      * Делаю неявную блокировку. для исключения ситуации когда один из потоков
      * merge завершиться самым первым до старта других потоков и начнется процесс получения картинок
      * для незаконченных потоков
      * @todo переписать с помощью мьтекса
      */
    this->incJobsCount("convert_pdf2png");

    for (int p_copy=VPrn::FirstPage; p_copy <= VPrn::FirstPageN5;p_copy++){
        copy_dir = QObject::trUtf8("%1/%2-copy").arg(workDir).arg(p_copy,0,10);
        QStringList t_files = findFilesInDir(copy_dir,QStringList()<<"*_out.pdf");
        for (int i=0; i< t_files.size();i++){
            QString in_file = t_files.at(i);
            // spoolDir/client_uuid/t_firstpage.pdf
            QRegExp rx(".+/(.-copy)/(.+)_out.pdf");
            if(rx.indexIn( in_file ) != -1){
                // Наш файлик можно обрабатывать
                QString copy_num = rx.cap(1);
                QString page_type  = rx.cap(2);
                QString out_file;
                if (mode){
                    out_file = QString("-sOutputFile=%1/%2/%3_\%d.png")
                                .arg(workDir,copy_num,page_type);
                }else{
                    out_file = QString("-sOutputFile=%1/%2/%3_0.png")
                                .arg(workDir,copy_num,page_type);
                }
                this->pdf2png(in_file,out_file,mode);
            }
        }
    }
    this->isJobFinished("convert_pdf2png");
}

//------------------------------------------ Private slots -----------------------------------------------
void GS_plugin::threadFinish(const QString &jobKey,int code,
                             const QString &output_message)
{
    VPrn::Jobs m_job = jobs_list.take(jobKey);
    if (code != 0 ){
        //Задача завершена с ошибкой, доложим наверх
        emit error(VPrn::InternalAppError,
                   QObject::trUtf8("При выполнении потока [%1] произошла ошибка %2 с кодом %3.")
                   .arg(m_job,0,10).arg(code,0,10).arg(output_message));
        return;
    }

    switch (m_job){
    case VPrn::job_ConvertPs2Pdf:
        emit docConvertedToPdf();
        this->calcPageCount(mainPdf);
        break;
    case VPrn::job_CalcPageCount:
        pCount = this->getPageCountFromProcOut(output_message);
        if ( pCount ==1){
            if ( !QFile::copy(mainPdf,firstPage )){
                emit error(VPrn::FileIOError,
                           QObject::trUtf8("Ошибка копирования документа PDF из %1 в %2\n%3")
                           .arg(mainPdf,firstPage).arg( QString(Q_FUNC_INFO) )
                           );
            }else{
                emit docReady4work(pCount);
            }
        }else{
            splitPdf();
        }
        break;
    case VPrn::job_SplitPageFirst:
        splitFirst = true;
        this->checkSplitState();
        break;
    case VPrn::job_SplitPageOther:
        splitOther = true;
        this->checkSplitState();
        break;
    case VPrn::job_MergePdf:
        if (this->isJobFinished("merge_pdf")){
            emit docMergedWithTemplate();
            qDebug() << Q_FUNC_INFO << " emit docMergedWithTemplate() ";
        }
        break;
    case VPrn::job_ConvertToPng:
        if (this->isJobFinished("convert_pdf2png")){
            emit generatePreViewFinished();
            qDebug() << Q_FUNC_INFO << " emit generatePreViewFinished() ";
        }
        break;
    default:
        break;
    }
}

//------------------------------------------ Private ------------------------------------------------------
void GS_plugin::recursiveDeletion(const QString &path)
{
    QDir dir(path);
    QStringList files = dir.entryList(QDir::Files);

    QStringList::Iterator itFile = files.begin();
    while (itFile != files.end())
        {
            QFile file(path + "/" + *itFile);
            if ( !file.remove()){
                emit error(VPrn::FileIOError,
                           QObject::trUtf8("Ошибка удаления файла %1!\n%2").arg(*itFile).arg(QString(Q_FUNC_INFO)));
            }
            ++itFile;
        }

    QStringList dirs = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
    QStringList::Iterator itDir = dirs.begin();
    while (itDir != dirs.end())
        {
            recursiveDeletion(path + "/" + *itDir);
            ++itDir;
        }

    if ( !dir.rmdir(path)){

        emit error(VPrn::FileIOError,
                   QObject::trUtf8("Ошибка удаления каталога %1!\n%2").arg(path).arg(QString(Q_FUNC_INFO)));

    }
}

void GS_plugin::createRCPfile()
{
    gs_rcp=QString("%1/%2.rcp").arg(workDir,
                                    QUuid::createUuid().toString().replace("{","").replace("}",""));
    QFile new_file;

    new_file.setFileName(gs_rcp);
    if (new_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&new_file);
        out     << QObject::trUtf8("-dQUIET\n")
                << QObject::trUtf8("-dNOPAUSE\n")
                << QObject::trUtf8("-dPARANOIDSAFER\n")
                << QObject::trUtf8("-dBATCH\n")
                << QObject::trUtf8("-r600\n")
                << QObject::trUtf8("-dPDFSETTINGS=/prepress\n")
                << QObject::trUtf8("-dEmbedAllFonts=true\n")
                << QObject::trUtf8("-sDEVICE=pdfwrite\n");
        new_file.close();
        emit pluginReady(workDir);
    }else{
        emit error(VPrn::FileIOError,
                   QObject::trUtf8("Ошибка создания файла %1\nОшибка %2\n%3")
                   .arg(gs_rcp)
                   .arg(new_file.errorString())
                   .arg(QString(Q_FUNC_INFO)));
        emit pluginNotReady();
    }
}

void GS_plugin::start_proc(const QString &bin,QStringList &arg_list,VPrn::Jobs job_id)
{
    // Формируем ключ и запишем данные в списки
    QString j_key = QUuid::createUuid().toString().replace("{","").replace("}","");
    jobs_list.insert(j_key,job_id);

    ProcessT *proc = new ProcessT(this,j_key);
    // Обработчик сообщений файлового потока
    connect(proc, SIGNAL(jobFinish     (const QString &,int,const QString &)),
            this, SLOT  (threadFinish  (const QString &,int,const QString &))
            );
    proc->addToEnv(myEnv);
    proc->execute(bin, arg_list,QProcess::MergedChannels);
}

void GS_plugin::calcPageCount(const QString &input_fn)
{
    //pdfTk input_file dump_data
    QStringList args;
    args.append(input_fn);
    args.append("dump_data");
    start_proc(pdftkBin,args,VPrn::job_CalcPageCount);
}

qint32 GS_plugin::getPageCountFromProcOut(const QString &out_str)
{
    int pagesCount(-1); // Число страниц в конвертируемом документе
    QRegExp rx;

    if (!out_str.isEmpty()) {
        rx.setPattern("NumberOfPages:\\s+(\\d{1,}).*");
        //rx.setMinimal(true);
        if (rx.indexIn(out_str) != -1) {
            bool ok;
            //qDebug() << Q_FUNC_INFO << rx.cap(0) << rx.cap(1);
            pagesCount = rx.cap(1).toInt(&ok, 10);
        }
    }
    return pagesCount;
}

void GS_plugin::splitPdf()
{
    /**
      * pdftk %1 cat %2 output page_%2.pdf
      * @brief Так как сюда попадаем только только если в документе больше 1 страницы,
      * То запускаем 2 потока формирирование 1 стр, и формирование последующих
      */
    // Первую страницу
    QStringList args;
    args.append(mainPdf);
    args.append("cat");
    args.append("1");
    args.append("output");
    args.append(firstPage);

    start_proc(pdftkBin,args,VPrn::job_SplitPageFirst);

    args.clear();
    args.append(mainPdf);
    args.append("cat");
    args.append("2-end");
    args.append("output");
    args.append(otherPage);

    start_proc(pdftkBin,args,VPrn::job_SplitPageOther);
}

void GS_plugin::checkSplitState()
{
    if ( splitFirst && splitOther && pCount != -1  ){
        emit docReady4work(pCount);
    }
}

void GS_plugin::catPdf(const QString &file_nameA,const QString &file_nameB,
                       const QString &output_name)
{

    QStringList args;
    args.append(QString("A=%1").arg(file_nameA));
    args.append(QString("B=%1").arg(file_nameB));
    args.append("cat A B output ");
    args.append(output_name);

    start_proc(pdftkBin,args,VPrn::job_CatPages);
}




void GS_plugin::mergePdf(const QString &in_pdf,
                         const QString &back_pdf,const QString &out_pdf)
{
    //pdftk in.pdf background back.pdf output out.pdf
    if (!QFile::exists(in_pdf)) {
        emit error(VPrn::FileIOError,
                   QObject::trUtf8("ERROR : Файл %1 не найден\n%2").arg(in_pdf).arg(QString(Q_FUNC_INFO)));
        return;
    }

    if (!QFile::exists(back_pdf)) {
        emit error(VPrn::FileIOError,
                   QObject::trUtf8("ERROR : Файл %1 не найден\n%2").arg(back_pdf).arg(QString(Q_FUNC_INFO)));
        return;
    }

    QStringList args;
    args.append(in_pdf);
    args.append("background");
    args.append(back_pdf);
    args.append("output");
    args.append(out_pdf);

    this->incJobsCount("merge_pdf");

    start_proc(pdftkBin,args,VPrn::job_MergePdf);
}

void GS_plugin::pdf2png(const QString &in_pdf,
                        const QString &out_pdf,bool mode)
{
    //gs -q -sDEVICE=png16m -dBATCH -dNOPAUSE -dFirstPage=1 -dLastPage=5 -r300 -sOutputFile=out_%d.png in.pdf
    this->incJobsCount("convert_pdf2png");
    QStringList args;
    args.append("-q");
    args.append("-dQUIET");
    args.append("-dNOPAUSE");
    args.append("-dBATCH");
    args.append("-dPARANOIDSAFER");
    /// @todo Добавить вычисление разрешения max (106*72/max_x,106*72/max_y)
    args.append("-r150");
    args.append("-sDEVICE=png16m");
    args.append("-dFirstPage=1");
    if (!mode){
        args.append("-dLastPage=1");
    }
    args.append(out_pdf);
    args.append(in_pdf);
    start_proc(gsBin,args,VPrn::job_ConvertToPng);
}

void GS_plugin::incJobsCount(const QString &job_id)
{
    if ( !metaJob_counter.contains(job_id) ){
        metaJob_counter.insert(job_id,0);
    }else{
        QMutableHashIterator<QString, int> i(metaJob_counter);
        while (i.hasNext()) {
            i.next();
            if (i.key().compare(job_id,Qt::CaseInsensitive)==0){
                int job_count = i.value();
                job_count++;
                i.setValue(job_count);
            }
            qDebug() << i.key() << ": " << i.value();
        }
    }
}

bool GS_plugin::isJobFinished(const QString &job_id)
{
    if ( metaJob_counter.contains(job_id) ){
        QMutableHashIterator<QString, int> i(metaJob_counter);
        while (i.hasNext()) {
            i.next();
            qDebug() <<"isJobFinished " << i.key() << ": " << i.value();

            if (i.key().compare(job_id,Qt::CaseInsensitive) ==0 ){
                int job_count = i.value();

                if (job_count == 0){
                    i.remove();
                    qDebug() <<"job " << job_id << " finish true" ;
                    return true;
                }else{
                    job_count--;
                    i.setValue(job_count);
                    return false;
                }
            }

        }
    }
    return false;
}

Q_EXPORT_PLUGIN2(gs_plugin, GS_plugin);

