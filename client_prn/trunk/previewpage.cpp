#include "previewpage.h"

#include "mytypes.h"
using namespace VPrn;

#include <QtGui/QVBoxLayout>
#include <QtGui/QRadioButton>
#include <QtGui/QCheckBox>
#include <QtGui/QMessageBox>
#include <QtGui/QSplitter>
#include <QtCore/QStringList>
#include <QStringListModel>
#include <QModelIndex>

#include <QRegExp>

PreViewPage::PreViewPage(QWidget *parent)
    : QWidget(parent)
    ,leftFrame(0)
    ,rightFrame(0)
    ,thumbSize(100,100)   
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
    this->setWindowTitle(QObject::trUtf8("Предварительный просмотр сформированного документа."));

    int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    QSize iconSize(size, size);

    leftFrame = new ViewPort(this);
    rightFrame = new ViewPort(this);

    leftFrame->showThumbNail(true,thumbSize);
    leftFrame->setMinimumSize(200,512);
    rightFrame->setMinimumSize(720,512);

    // Zoom slider layout
    QFrame *zoomSliderFrame = new QFrame();
    zoomSliderFrame->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);

    QToolButton *zoomToFitIcon = new QToolButton;
    zoomToFitIcon->setAutoRepeat(true);
    zoomToFitIcon->setAutoRepeatInterval(33);
    zoomToFitIcon->setAutoRepeatDelay(0);
    zoomToFitIcon->setIcon(QPixmap(":/zoomtofit.xpm"));
    zoomToFitIcon->setIconSize(iconSize);
    zoomToFitIcon->setToolTip(QObject::trUtf8("Увеличить изображение до размеров экрана."));

    QToolButton *zoomToFullSizeIcon = new QToolButton;
    zoomToFullSizeIcon->setAutoRepeat(true);
    zoomToFullSizeIcon->setAutoRepeatInterval(33);
    zoomToFullSizeIcon->setAutoRepeatDelay(0);
    zoomToFullSizeIcon->setIcon(QPixmap(":/zoomtofull.xpm"));
    zoomToFullSizeIcon->setIconSize(iconSize);
    zoomToFullSizeIcon->setToolTip(QObject::trUtf8("Исходный размер изображения."));

    QToolButton *zoomInIcon = new QToolButton;
    zoomInIcon->setAutoRepeat(true);
    zoomInIcon->setAutoRepeatInterval(33);
    zoomInIcon->setAutoRepeatDelay(0);
    zoomInIcon->setIcon(QPixmap(":/zoomin.xpm"));
    zoomInIcon->setIconSize(iconSize);
    zoomInIcon->setToolTip(QObject::trUtf8("Уменьшить масштаб изображения."));

    QToolButton *zoomOutIcon = new QToolButton;
    zoomOutIcon->setAutoRepeat(true);
    zoomOutIcon->setAutoRepeatInterval(33);
    zoomOutIcon->setAutoRepeatDelay(0);
    zoomOutIcon->setIcon(QPixmap(":/zoomout.xpm"));
    zoomOutIcon->setIconSize(iconSize);
    zoomOutIcon->setToolTip(QObject::trUtf8("Увеличить масштаб изображения."));
    zoomSlider = new QSlider();
    zoomSlider->setMinimum(0);
    zoomSlider->setMaximum(500);
    zoomSlider->setValue(250);
    zoomSlider->setOrientation(Qt::Horizontal);
    zoomSlider->setTickPosition(QSlider::TicksRight);

    QHBoxLayout * zoomSliderFrame_layout = new QHBoxLayout();
    zoomSliderFrame->setLayout(zoomSliderFrame_layout);
    zoomSliderFrame_layout->addWidget(zoomToFullSizeIcon);
    zoomSliderFrame_layout->addWidget(zoomToFitIcon);
    zoomSliderFrame_layout->addWidget(zoomInIcon);
    zoomSliderFrame_layout->addWidget(zoomSlider);
    zoomSliderFrame_layout->addWidget(zoomOutIcon);

    QSplitter    *toolSplitter = new QSplitter();
    toolSplitter->setOrientation(Qt::Vertical);
    toolSplitter->addWidget(zoomSliderFrame);
    toolSplitter->addWidget(rightFrame);
    toolSplitter->setStretchFactor(1, 1);

    QSplitter    *mainSplitter = new QSplitter();
    mainSplitter->setOrientation(Qt::Horizontal);
    mainSplitter->addWidget(leftFrame);
    mainSplitter->addWidget(toolSplitter);
    mainSplitter->setStretchFactor(1, 1);


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget ( mainSplitter);
    setLayout(layout);

    connect(leftFrame,SIGNAL(selectPixmap(int)),
            rightFrame,SLOT(showPage(int)));

    connect(zoomInIcon, SIGNAL(clicked()),
            this, SLOT(zoomIn())
            );
    connect(zoomOutIcon, SIGNAL(clicked()),
            this, SLOT(zoomOut())
            );
    connect(zoomSlider, SIGNAL(valueChanged(int)),
            rightFrame, SLOT(setupMatrix(int))
            );
    connect (zoomToFullSizeIcon,SIGNAL(clicked()),
             rightFrame,SLOT(zoomToFullSize()));
    connect (zoomToFitIcon,SIGNAL(clicked()),
             rightFrame,SLOT(zoomToFit()));


}


void PreViewPage::setModel(QStringListModel *m)
{
    model = m;    
}

void PreViewPage::needRestart()
{    
    imageFullItems.clear();
    imageThumbItems.clear();
    descImagesList.clear();
    imageFilesList.clear();
}


void PreViewPage::updatePictures()
{

    for (int i=0;i<model->rowCount();i++){
        QModelIndex idx = model->index(i,0);
        QString pic_name = model->data(idx,Qt::DisplayRole).toString();
        QPixmap img_item;
        if (img_item.load(pic_name)){
            descImagesList.insert(i, this->getDescPage(pic_name) );
            imageFullItems.insert(i,img_item);

            imageThumbItems.insert(i,img_item.scaled(thumbSize,
                                                     Qt::KeepAspectRatio,
                                                     Qt::SmoothTransformation));
        }
    }

    leftFrame->setDescList(descImagesList);
    leftFrame->setPixMapList(imageThumbItems);
    rightFrame->setPixMapList(imageFullItems);
    emit enablePrintDoc();

}


//------------------------------ PRIVATE SLOTS ---------------------------------
void PreViewPage::zoomIn()
{
    zoomSlider->setValue(zoomSlider->value() - 1);
}

void PreViewPage::zoomOut()
{
    zoomSlider->setValue(zoomSlider->value() + 1);
}


QString PreViewPage::getDescPage(const QString &f_name) const
{
    QRegExp rx("(.+)/(.+)-copy/(.+)_(\\d{1,}).png");

    QString str_type;

    if(rx.indexIn( f_name ) != -1){
        // Наш файлик можно обрабатывать
        QString copy_num = rx.cap(2);
        QString page_type  = rx.cap(3);
        QString page_number  = rx.cap(4);

        if (page_type.compare("firstpage") == 0){
            str_type = QString("<center>Экз.№%1<br/>%2[%3]</center>")
                    .arg(copy_num,
                         QObject::trUtf8("Лицевая сторона 1-й лист."),
                         page_number );
        }
        if (page_type.compare("lastpage") == 0){
            str_type = QString("<center>Экз.№%1<br/>%2[%3]</center>")
                    .arg(copy_num,
                         QObject::trUtf8("Фонарик"),
                         page_number );
        }
        if (page_type.compare("otherpage") == 0){
            str_type = QString("<center>Экз.№%1<br/>%2[%3]</center>")
                    .arg(copy_num,
                         QObject::trUtf8("Лицевая сторона 2-й и далее листы.")
                         ,page_number  );
        }

        if (page_type.compare("oversidepage") == 0){
            str_type = QString("<center>Экз.№%1<br/>%2[%3]</center>")
                    .arg(copy_num,
                         QObject::trUtf8("Обратная сторона."),
                         page_number  );
        }
    }
    return str_type;
}


