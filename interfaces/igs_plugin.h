#ifndef IGS_PLUGIN_H
#define IGS_PLUGIN_H
#include "mytypes.h"

using namespace VPrn;

QT_FORWARD_DECLARE_CLASS(QString)
QT_FORWARD_DECLARE_CLASS(QByteArray)

class Igs_plugin{

public:
    virtual ~Igs_plugin() {}
    virtual void init(const QString &gs_bin,const QString &pdftk_bin,
                      const QString &temp_folder)=0;
    virtual void clear()=0; 	
    virtual void convertPs2Pdf(const QString &input_fn) = 0;
    virtual void catPdf(const QString &file_nameA,const QString &file_nameB,
                        const QString &output_name) =0;
    virtual void mergeWithTemplate() = 0;
    virtual void docMergedWithTemplate() =0;
    virtual void generatePreViewFinished() =0;
    virtual void convertPdf2Png(bool mode) = 0;
    virtual QStringList findFilesInDir(const QString &tDir, /* Каталог поиска*/
                               const QStringList &filters /*Тип файлов*/
                               ) const =0;
    /*

    virtual void convertPdfToPng(const QString &client_uuid,
                                     const QStringList &files) = 0;
    virtual void final_clear() = 0;
    virtual void mergePdf(const QString &client_uuid,
                          const QString &in_pdf,
                          const QString &back_pdf,
                          const QString &out_pdf) = 0;

    virtual void directPrint(const QString &client_uuid,const QString &file_name,
                              const QString &printer_name,int copies) = 0;

    virtual void mergeWithTemplate(const QString &client_uuid,
                                   const QStringList &t_files,
                                   VPrn::PreviewMode prn_mode) = 0 ;

    virtual void setConvertToPngMode(const QString &client_uuid, bool full_doc ) = 0;
    virtual QStringList findFiles(const QString &client_uuid,const QStringList &filters) = 0;
    virtual QStringList findFiles4Copy(const QString &client_uuid,
                               int copyNum,
                               const QStringList &filters
                               ) = 0;
    virtual QString getUuid() const = 0;    
    virtual void calcPageCount(const QString &client_uuid,const QString &input_fn = QString() )= 0;
    virtual void catPdf(const QString &client_uuid,const QString &file_nameA,
                       const QString &file_nameB, const QString &output_name) =0;

    */
};


QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Igs_plugin,"com.Zarya.Plugin.Igs_plugin/2.0");
QT_END_NAMESPACE


#endif
