#ifndef MY_TYPES_H
#define MY_TYPES_H

#include <QtCore/QMetaType>
#include <QtCore/QMap>
#include <QtGui/QPixmap>


namespace VPrn{

#define MY_DEBUG
#define POINT_TO_CM(cm) ((cm)/28.3465058)
#define POINT_TO_MM(mm) ((mm)/2.83465058)     ////////////  0.352777778
#define POINT_TO_DM(dm) ((dm)/283.465058)
#define POINT_TO_INCH(inch) ((inch)/72.0)

#define MM_TO_POINT(mm) ((mm)*2.83465058)
#define CM_TO_POINT(cm) ((cm)*28.3465058)     /////// 28.346456693
#define DM_TO_POINT(dm) ((dm)*283.465058)
#define INCH_TO_POINT(inch) ((inch)*72.0)

    /// @todo  Хардкодим по маленьку
    static const qint32 m_MaxInt32= 2147483647;

    typedef qint32 PACKET_SIZE ; // Максимальный размер пакета

    typedef QMap <int,QPixmap> PixmapList;
    typedef QMap <int,QString> PixmapDescList;

    static const int ObjectName = 0;
    static const int ObjectData = 1;
    static const int format = 13;

    static const char PARTSEPARATOR = ':';
    static const char MESSAGESEPARATOR = '|';

    struct Printers {
        QString name;       //Имя принтера (для пользователя)
        QString ip;         //IP адрес сервера где размещен принтер
        QString p_qqueue;   //Имя очереди печати (имя принтера в CUPS)
    };

    typedef QList<Printers> PrinterList;

    enum EventLogMessageId{
        //ErrorMessages
        eId_EmptyMessage      = 0,
        eId_AppsError         = 10,    // Внутрення ошибка приложения
        eId_FileNotFound      = 1000,
        eId_RecivedPacketSize = 2000,
        eId_SocketErrorDesc   = 2001,
        eId_CanNotBindPort    = 2003,
        eId_CanNotReadConfig  = 2004,
        eId_BrokenConfigFile  = 2005,
        eId_ServiceNotStarted = 2006,
        eId_NetworkError      = 2007,
        eId_SQL_CoreNotInited = 2008,
        eId_SQL_ExecError     = 2009,

        //Information Messages        
        eId_ServiceStarted    = 4000,
        eId_ServiceStoped     = 4001,
        eId_ServicePaused     = 4002,
        eId_ServiceResume     = 4003,
        eId_SQL_CoreInited    = 4004,

        // DebugMessages
        eId_DebugInfo = 7000
                    };

    enum EventLogType{
        Success = 0, Error, Warning, Information
              };

    enum EventLogCategory{
        eCatId_Empty       = 0x0,
        eCatId_Error       = 0x1,
        eCatId_Information = 0x2,
        eCatId_DebugInfo   = 0x3
                         };
    enum pSizeColumnOrder{
        pSize_id     = 0, /// ID
        pSize_page   = 1, /// размер листа
        pSize_code   = 2, /// Код
        pSize_width  = 3, /// Ширина (мм)
        pSize_height = 4  /// Высота (мм)
                   };
/*
    enum metaInfoOrder{
        metaInfo_name,    /// имя шаблона
        metaInfo_fname,   /// file_name шаблона
        metaInfo_desc,    /// Описание шаблона;
        metaInfo_orient,  /// Ориентация (Книжная/Альбомная)
        metaInfo_ctime,   /// Время создания
        metaInfo_mtime,   /// Время изменения
        metaInfo_author,  /// Автор
        metaInfo_mtop,    /// Отступ сверху (мм)
        metaInfo_mbottom, /// Отступ снизу (мм)
        metaInfo_mleft,   /// Отступ слева (мм)
        metaInfo_mright,  /// Отступ справа (мм)
        metaInfo_p_width, /// Ширина страницы (мм)
        metaInfo_p_height /// Высота страницы (мм)
    };

    enum DocCardOrder{
        cards_ID              = 0,  /// ID
        cards_DOC_NAME        = 1,  /// Имя документа
        cards_STAMP           = 2,  /// Выбранный гриф секретности
        cards_MB_NUMBER       = 3,  /// Номер МБ
        cards_PUNKT           = 4,  /// Пункт
        cards_PAGE_COUNT      = 5,  /// Число страниц
        cards_COPY_COUNT      = 6,  /// Всего экз
        cards_CURRENT_COPY    = 7,  /// Текущий экз
        cards_SELECT_ALL_COPY = 8,  /// Признак выбор всех экз
        cards_TEMPLATE_NAME   = 9,  /// Имя файла шаблона
        cards_EXECUTOR        = 10, /// Исполнитель
        cards_PRINTMAN        = 11, /// Отпечатал
        cards_PHONE           = 12, /// Телефон
        cards_INV_NUMBER      = 13, /// Инв.номер
        cards_PRINT_DATE      = 14, /// Дата печати
        cards_RECIVER_1       = 15, /// Получатель_1
        cards_RECIVER_2       = 16, /// Получатель_2
        cards_RECIVER_3       = 17, /// Получатель_3
        cards_RECIVER_4       = 18, /// Получатель_4
        cards_RECIVER_5       = 19, /// Получатель_5
        cards_STATUS          = 20  /// Статус_Документа
                            };



    enum tInfoColumnOrder{
        tInfo_id     = 0,  ///ID
        tInfo_name   = 1,  ///Имя шаблона
        tInfo_desc   = 2,  ///Описание шаблона;
        tInfo_pageID = 3,  ///Ссылка на таблицу размеры страниц
        tInfo_angle  = 4,  ///Поворот (град.)
        tInfo_ctime  = 5,  ///Время создания
        tInfo_mtime  = 6,  ///Время изменения
        tInfo_author = 7, ///Автор
        tInfo_mtop   = 8, ///Отступ сверху (мм)
        tInfo_mbottom = 9,///Отступ снизу (мм)
        tInfo_mleft   = 10,///Отступ слева (мм)
        tInfo_mright  = 11, ///Отступ справа (мм)
        tInfo_p_width = 12,
        tInfo_p_height= 13
                    };
*/
    enum pageDetailColumnOrder{
        PD_id         = 0,  /// ID
        PD_p_number   = 1,  /// Порядковый номер в шаблоне
        PD_p_name     = 2,   /// Имя страницы
        PD_p_visible  = 3   /// Видна страница да/нет 1/0
                    };

    enum elemColumnOrder{
        elem_id    = 0,  ///ID
        elem_text  = 1,  ///Текст отображаемый на экране и на принтере
        elem_tag   = 2,  ///Тэг с которым работает программа
        elem_pos_x = 3,  ///Позиция элемента на сцене х
        elem_pos_y = 4,  ///Позиция элемента на сцене у
        elem_color = 5,  ///Цвет шрифта
        elem_font  = 6,  ///Шрифт
        elem_angle = 7,  ///Угол поворота относиттельно сцены
        elem_border= 8,  /// исовать границу элемента, да/нет
        elem_img_data = 9, /// Если эемент картинка то тут будет записанны данные в виде бинарных данных
        elem_img_scaled =10, /// масштаб картинки
        elem_always_view = 11, /// Всегда отображаемый Да/нет
        elem_p_number    = 12,  /// Порядковый номер страницы 0-7 на которой виден элемент
        elem_text_img    = 13  /// Тип элемента текст или картинка(1/0)
                       };

    enum AppErrorType {
        NoError,
        FileNotFound,
        FileIOError,
        UnknownError,
        SqlDriverNotLoad,
        NotConnectedToDB,
        DBOpeningError,
        SQLQueryError,
        SQLCommonError,
        InternalAppError,
        AuthCommonError,
        NetworkError,
        PluginsNotLoad,
        SocketError,
        ConfigError,
        notDefainedMode,
        appNotStarted,
        TemplateNotLoad
    };

    enum pageNumbers {
        FirstPage        = 1,
        FirstPageN2      = 2,
        FirstPageN3      = 3,
        FirstPageN4      = 4,
        FirstPageN5      = 5,
        SecondPage       = 6,
        ThirdPage        = 7,
        FourthPage       = 8,
    };

    enum MessageType {
        Que_GetPrintersInfo = 9000,
        Ans_GetPrintersInfo = 9010,
        
        Cmd_SaveUserToBase  = 9011, 
        Ans_userListSavedToBase = 9012,
        ///Служебные сообщения
        Cmd_FinishWork    = 70000,        /// Команда удаленому клиенту завершить работу
        Err_Message      = 70010,        /// Сообщение об ошибке.Подробности в теле сообщения
        NoMsgType        = 0
                       };


}

Q_DECLARE_METATYPE(VPrn::AppErrorType);
Q_DECLARE_METATYPE(VPrn::MessageType);
Q_DECLARE_METATYPE(VPrn::pSizeColumnOrder);
Q_DECLARE_METATYPE(VPrn::elemColumnOrder);
Q_DECLARE_METATYPE(VPrn::pageDetailColumnOrder);
Q_DECLARE_METATYPE(VPrn::pageNumbers);
#endif

