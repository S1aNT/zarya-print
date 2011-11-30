#include "templatesdatagateway.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQueryModel>
#include <QList>
#include <QStandardItem>
#include <QModelIndex>
#include <QDir>
#include <QPrinter>

#include "myscene.h"
//#include <QTableView>

TemplatesDataGateway::TemplatesDataGateway(QObject *parent)
    : QObject(parent)
    , t_model(0)
    , card_model(0)
    , pages_model(0)
    , sqlDrvLoad(false)
{
    // Инициализация БД, проверка наличия требуемого драйвера
    QSqlDatabase db  = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"),
                                                 QLatin1String("TINFO_CON"));

    if (db.driver()->lastError().type() != QSqlError::ConnectionError) {
        sqlDrvLoad = true;
        emit sendEventMessageInfo(QObject::trUtf8("Драйвер sqlite, успешно загружен."),
                                  VPrn::eId_SQL_CoreInited,
                                  VPrn::Information,
                                  VPrn::eCatId_DebugInfo);

    }else{
        emit sendEventMessageInfo(QObject::trUtf8("Драйвер sqlite, загрузить не удалось!"),
                                  VPrn::eId_SQL_CoreNotInited,
                                  VPrn::Error,
                                  VPrn::eCatId_DebugInfo);

    }
    pages_model = new QSqlQueryModel(this);

    //    QTableView *t1 = new QTableView();
    //    t1->setModel(pages_model);
    //    t1->show();

}

TemplatesDataGateway::~TemplatesDataGateway()
{
    {
        QSqlDatabase db  = QSqlDatabase::database(QLatin1String("TINFO_CON"),true);
        if (db.isOpen()){
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(QLatin1String("TINFO_CON"));
}

void TemplatesDataGateway::setWorkingDir(const QString &wDir)
{
    workDir = wDir;
    QDir dir;
    for (int i=1;i<6;i++){
        QString copy_dir = QObject::trUtf8("%1/%2-copy").arg(wDir).arg(i,0,10);
        if (!dir.mkpath(copy_dir)){
            emit sendEventMessageInfo(QObject::trUtf8("Ошибка создания каталога [%1]!").arg(copy_dir),
                                      VPrn::eId_AppsError,
                                      VPrn::Error,
                                      VPrn::eCatId_DebugInfo);

        }
    }
}

void TemplatesDataGateway::getMetaInfo(const QStringList &list)
{
    if (!t_model || list.isEmpty() ){
        return;
    }
    for (int i = 0; i<list.size(); i++){
        this->readMeta(list.at(i));
    }
}

void TemplatesDataGateway::prepareTemplate(const QString &t_fileName)
{
    // Подготовим соединение с шаблоном как с БД
    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("TINFO_CON"),false);
    db.setDatabaseName( t_fileName );
    int angle;
    qreal width;
    qreal height;
    this->getPageInfoFromModel(t_fileName,angle,width,height);

    if (db.open()){
        if ( this->saveModelToBase()){
            pages_model->setQuery(QString("SELECT p_number FROM page_detail WHERE p_visible =1"),db);
            if (pages_model->lastError().isValid()){
                this->dumpError(pages_model->lastError());
            }else{
                for (int p_copy=VPrn::FirstPage; p_copy <= VPrn::FirstPageN5;p_copy++){
                    if (this->isCopyValid(p_copy)){
                        if (this->updateElementsTable(QObject::trUtf8("Номер экз."),QObject::tr("%1").arg(p_copy,0,10))){
                            //Формируем шаблон для данного экземпляра
                            for (int i=0;i<pages_model->rowCount();i++){
                                QString pName;
                                QModelIndex idx = pages_model->index(i,0);
                                int page_number = pages_model->data(idx,Qt::DisplayRole).toInt();
                                if (page_number < VPrn::FourthPage  && page_number == p_copy){ // одна из копий
                                    pName = QObject::tr("%1/%2-copy/t_%3.pdf").arg(workDir).arg(p_copy,0,10).arg("firstpage");
                                }else{
                                    if (page_number == VPrn::SecondPage){
                                        pName = QObject::tr("%1/%2-copy/t_%3.pdf").arg(workDir).arg(p_copy,0,10).arg("otherpage");
                                    }
                                    if (page_number == VPrn::ThirdPage){
                                        pName =QObject::tr("%1/%2-copy/t_%3.pdf").arg(workDir).arg(p_copy,0,10).arg("oversidepage");
                                    }
                                    if (page_number == VPrn::FourthPage){
                                        pName = QObject::tr("%1/%2-copy/t_%3.pdf").arg(workDir).arg(p_copy,0,10).arg("lastpage");
                                    }
                                }
                                if (!pName.isEmpty()){
                                    this->printPageToPdf(page_number,angle,width,height,pName);
                                }
                            }
                        }
                    }
                }
                emit finishGenTemplate();
            }
        }
    }
    db.close();
}

//------------------------------ Public slots ---------------------------------------------------


//------------------------------ Private --------------------------------------------------------

void TemplatesDataGateway::readMeta(const QString &fileName)
{   

    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("TINFO_CON"),false);

    db.setDatabaseName( fileName );
    if (db.open()){
        QSqlQuery query(db);
        QList<QStandardItem *> itemList;
        if (query.exec( "SELECT t_name,t_desc,"
                       "angle,c_time,m_time,author,margin_top,margin_bottom,"
                       "margin_left,margin_right,page_size.p_width,page_size.p_height  "
                       "FROM template "
                       " INNER JOIN page_size ON template.page_size_id=page_size.id")){

            int field_t_name   = query.record().indexOf("t_name");
            int field_t_desc   = query.record().indexOf("t_desc");
            int field_angle    = query.record().indexOf("angle");
            int field_c_time   = query.record().indexOf("c_time");
            int field_m_time   = query.record().indexOf("m_time");
            int field_author   = query.record().indexOf("author");
            int field_m_top    = query.record().indexOf("margin_top");
            int field_m_bottom = query.record().indexOf("margin_bottom");
            int field_m_left   = query.record().indexOf("margin_left");
            int field_m_right  = query.record().indexOf("margin_right");
            int field_p_width  = query.record().indexOf("p_width");
            int field_p_height = query.record().indexOf("p_height");
            while (query.next()) {

                itemList.append(new QStandardItem ( query.value(field_t_name).toString() )   );
                itemList.append(new QStandardItem ( fileName )                               );
                itemList.append(new QStandardItem ( query.value(field_t_desc).toString() )   );
                itemList.append(new QStandardItem ( query.value(field_angle).toString() )    );
                itemList.append(new QStandardItem ( query.value(field_c_time).toString() )   );
                itemList.append(new QStandardItem ( query.value(field_m_time).toString() )   );
                itemList.append(new QStandardItem ( query.value(field_author).toString() )   );
                itemList.append(new QStandardItem ( query.value(field_m_top).toString() )    );
                itemList.append(new QStandardItem ( query.value(field_m_bottom).toString() ) );
                itemList.append(new QStandardItem ( query.value(field_m_left).toString() )   );
                itemList.append(new QStandardItem ( query.value(field_m_right).toString() )  );
                itemList.append(new QStandardItem ( query.value(field_p_width).toString() )  );
                itemList.append(new QStandardItem ( query.value(field_p_height).toString() ) );
            }
            t_model->appendRow(itemList);
        }else{
            this->dumpError(query.lastError());
        }
        db.close();
    }

}

void TemplatesDataGateway::dumpError (const QSqlError & lastError)
{
    emit sendEventMessageInfo(QObject::trUtf8("Ошибка работы с БД sqlite. %1")
                              .arg( lastError.text() ),
                              VPrn::eId_SQL_ExecError,
                              VPrn::Error );

    qDebug()<< "lastError.text() "        << lastError.text() << "\n"
            << "lastError.databaseText() "<< lastError.databaseText() << "\n"
            << "lastError.driverText()"   << lastError.driverText() << "\n"
            << "lastError.type() "        << lastError.type() << "\n"
            << "lastError.number() "      << lastError.number() << "\n";
}

void TemplatesDataGateway::getPageInfoFromModel(const QString &fileName,int &p_angle,qreal &p_width, qreal &p_height)
{    
    if (t_model){
        for (int i=0; i <t_model->rowCount();i++){
            QModelIndex idx =t_model->index(i,VPrn::metaInfo_fname);
            if (fileName.compare(t_model->data(idx,Qt::DisplayRole).toString(),Qt::CaseInsensitive) ==0 ){
                // Получим размер страницы и ее ориентацию
                p_angle = t_model->data ( t_model->index(i,VPrn::metaInfo_orient),Qt::DisplayRole).toInt();
                p_width = MM_TO_POINT ( t_model->data(t_model->index(i,VPrn::metaInfo_p_width),Qt::DisplayRole).toDouble() );
                p_height= MM_TO_POINT ( t_model->data(t_model->index(i,VPrn::metaInfo_p_height),Qt::DisplayRole).toDouble() );
                break;
            }
        }
    }else{
        emit sendEventMessageInfo(QObject::trUtf8("Модель данных ШАБЛОНЫ отсутсвует или повреждена!"),
                                  VPrn::eId_AppsError,
                                  VPrn::Error,
                                  VPrn::eCatId_DebugInfo);
    }    
}

bool TemplatesDataGateway::saveModelToBase()
{
    /// @todo ошибки при записи в базу неверные значения
    if( card_model ){
        QSqlDatabase db  = QSqlDatabase::database(QLatin1String("TINFO_CON"),true);

        bool ok=true;
        {
            for (int i=0;i<card_model->rowCount();i++){
                QString tag;
                for (int j=0;j<card_model->columnCount();j++){
                    switch (j){
                    case VPrn::cards_DOC_NAME:
                        tag = QObject::trUtf8("Название док-та");
                        break;
                    case VPrn::cards_STAMP_NAME:
                        tag = QObject::trUtf8("Гриф");
                        break;
                    case VPrn::cards_MB_NUMBER:
                        tag = QObject::trUtf8("МБ");
                        break;
                    case VPrn::cards_PUNKT:
                        tag =QObject::trUtf8("Пункт перечня");
                        break;
                    case VPrn::cards_PAGE_COUNT:
                        tag = QObject::trUtf8("Кол-во листов");
                        break;
                    case VPrn::cards_COPY_COUNT:
                        tag = QObject::trUtf8("Кол-во экз.");
                        break;
                    case VPrn::cards_CURRENT_COPY:
                        tag = QObject::trUtf8("Номер экз.");
                        break;
                    case VPrn::cards_EXECUTOR:
                        tag = QObject::trUtf8("Исполнитель");
                        break;
                    case VPrn::cards_PRINTMAN:
                        tag = QObject::trUtf8("Отпечатал");
                        break;
                    case VPrn::cards_PHONE:
                        tag = QObject::trUtf8("Телефон");
                        break;
                    case VPrn::cards_INV_NUMBER:
                        tag = QObject::trUtf8("Инв. N");
                        break;
                    case VPrn::cards_PRINT_DATE:
                        tag = QObject::trUtf8("Дата распечатки");
                        break;
                    case VPrn::cards_RECIVER_1:
                        tag = QObject::trUtf8("Получатель N1");
                        break;
                    case VPrn::cards_RECIVER_2:
                        tag = QObject::trUtf8("Получатель N2");
                        break;
                    case VPrn::cards_RECIVER_3:
                        tag = QObject::trUtf8("Получатель N3");
                        break;
                    case VPrn::cards_RECIVER_4:
                        tag = QObject::trUtf8("Получатель N4");
                        break;
                    case VPrn::cards_RECIVER_5:
                        tag = QObject::trUtf8("Получатель N5");
                        break;
                    default:
                        break;
                    }
                    /// @todo replace to prepare ... bind ... exec
                    ok = this->updateElementsTable(tag,this->getDataFromCell(i,j).toString());
                }
            }
        }
        return ok;
    }else{
        emit sendEventMessageInfo(QObject::trUtf8("Отсутствует модель данных [Карточка документа]"),
                                  VPrn::eId_AppsError,
                                  VPrn::Error,
                                  VPrn::eCatId_DebugInfo);

    }
    return false;
}

QVariant TemplatesDataGateway::getDataFromCell(int row,int col)
{
    QModelIndex idx = card_model->index(row,col);
    return card_model->data(idx,Qt::DisplayRole);
}

bool TemplatesDataGateway::isCopyValid(int pCopy)
{
    if (pCopy>VPrn::FirstPageN5){
        return false;
    }
    QModelIndex idx;
    switch (pCopy){
    case 1:
        idx = card_model->index(0,VPrn::cards_RECIVER_1);
        break;
    case 2:
        idx = card_model->index(0,VPrn::cards_RECIVER_2);
        break;
    case 3:
        idx = card_model->index(0,VPrn::cards_RECIVER_3);
        break;
    case 4:
        idx = card_model->index(0,VPrn::cards_RECIVER_4);
        break;
    case 5:
        idx = card_model->index(0,VPrn::cards_RECIVER_5);
        break;

    }

    QModelIndex pages_idx = pages_model->index(pCopy,0);

    qDebug() << "pCopy "    << pCopy;
    qDebug() << "card_model->data(idx,Qt::DisplayRole).toString() "
             << card_model->data(idx,Qt::DisplayRole).toString();
    qDebug() << "pages_model->data(idx,Qt::DisplayRole).toInt() = "
             << pages_model->data(pages_idx,Qt::DisplayRole).toBool();

    bool ok = !card_model->data(idx,Qt::DisplayRole).toString().isEmpty() &&
            pages_model->data(pages_idx,Qt::DisplayRole).toBool();

    return ok;
}



bool TemplatesDataGateway::updateElementsTable(const QString &m_tag, const QString &m_value)
{

    if (m_tag.isEmpty() || m_value.isEmpty()){
        return false;
    }

    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("TINFO_CON"),true);
    QSqlQuery query(db);
    QString q_str = QObject::trUtf8("UPDATE elem SET e_text='%1' WHERE img = 0 AND e_tag ='%2' ")
            .arg(m_value,m_tag);
    bool ok = query.exec( q_str );
    if (!ok){
        this->dumpError(query.lastError());
    }
    return ok;
}


void TemplatesDataGateway::printPageToPdf(int pages_number,int angle,qreal width,qreal height,const QString &fileName)
{
    QSqlDatabase db  = QSqlDatabase::database(QLatin1String("TINFO_CON"),true);
    QSqlQuery elrm_query(db);
    QString sub_query = QString("SELECT elem.id,e_text,e_tag,pos_x,pos_y,color,font,"
                                "angle,border,img_data,img_scaled, always_view,page_detail.p_number,img"
                                " FROM elem "
                                " INNER JOIN page_detail ON page_detail_id = page_detail.id "
                                " WHERE page_detail.p_visible = 1 AND  page_detail.p_number ='%1'"
                                " ORDER BY page_detail.p_number").arg(pages_number,0,10);

    if (elrm_query.exec( sub_query )){

        myScene *m_scene =new myScene(this);
        m_scene->createPageForPrint(width,height);

        QPrinter pdfprinter;
        pdfprinter.setOutputFileName(fileName);
        pdfprinter.setFullPage(true);
        pdfprinter.setOutputFormat( QPrinter::PdfFormat);
        pdfprinter.setResolution( QPrinter::HighResolution );
        if (angle == 90){
            pdfprinter.setOrientation(QPrinter::Landscape);
        }else{
            pdfprinter.setOrientation(QPrinter::Portrait);
        }
        int field_text        =  elrm_query.record().indexOf("e_text");
        int field_tag         =  elrm_query.record().indexOf("e_tag");
        int field_pos_x       =  elrm_query.record().indexOf("pos_x");
        int field_pos_y       =  elrm_query.record().indexOf("pos_y");
        int field_color       =  elrm_query.record().indexOf("color");
        int field_font        =  elrm_query.record().indexOf("font");
        int field_angle       =  elrm_query.record().indexOf("angle");
        int field_img_data    =  elrm_query.record().indexOf("img_data");
        int field_img_scaled  =  elrm_query.record().indexOf("img_scaled");
        int field_text_img    =  elrm_query.record().indexOf("text_img");

        while (elrm_query.next()) {
            int e_type  = elrm_query.value(field_text_img).toInt();
            qreal pos_x = elrm_query.value(field_pos_x).toDouble();
            qreal pos_y = elrm_query.value(field_pos_y).toDouble();
            QPointF ps(pos_x,pos_y);
            qreal e_angle      = elrm_query.value(field_angle).toDouble();
            if (e_type == 0){
                QString text     = elrm_query.value(field_text).toString();
                QString tag      = elrm_query.value(field_tag).toString();
                QVariant variant = elrm_query.value(field_color);
                QColor color     = variant.value<QColor>();
                variant          = elrm_query.value(field_font);
                QFont font       = variant.value<QFont>();

                m_scene->addBaseElem(tag,text,ps,font,color,e_angle);
            }else{
                //Читаем картинку из базы
                qreal scale_xy   = elrm_query.value(field_img_scaled).toDouble();
                QPixmap img;
                img.fill(Qt::transparent);
                img.loadFromData( elrm_query.value(field_img_data).toByteArray() );

                m_scene->addImgElem(ps,angle,scale_xy,img);
            }
        }
        // Если последний режим был показа тегов, то переключим на показ значений
        if (m_scene->getViewMode()){
            m_scene->setViewMode();
        }

        //Печать в pdf
        QPainter painter(&pdfprinter);
        painter.setOpacity(0.0);
        m_scene->render(&painter);

    }else{
        this->dumpError(elrm_query.lastError());
    }
}



