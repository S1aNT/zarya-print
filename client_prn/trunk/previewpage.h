#ifndef PREVIEWPAGE_H
#define PREVIEWPAGE_H

#include "viewport.h"

#include <QtGui/QWidget>
#include <QtGui/QPixmap>
#include <QtGui/QSlider>

#include <QtCore/QMap>

QT_FORWARD_DECLARE_CLASS ( QLabel )
QT_FORWARD_DECLARE_CLASS ( QCheckBox )
QT_FORWARD_DECLARE_CLASS (QStringListModel)



class PreViewPage : public QWidget
{
    Q_OBJECT
public:
    explicit PreViewPage(QWidget *parent = 0);
    void needRestart();

    bool enableNext();
    void setModel(QStringListModel *m);

signals:
    //void printCurrentDoc();
    void enablePrintDoc();

public slots:
    void updatePictures();

private slots:
    /**
      * @fn void zoomIn();
      * @fn void zoomOut();
      * @short Изменение масштаба на 1 ед.
      */
    void zoomIn();
    void zoomOut();


private:

    ViewPort *leftFrame;
    ViewPort *rightFrame;
    QSize thumbSize;
    QLabel *topLabel;
    PixmapList imageFullItems;
    PixmapList imageThumbItems;
    PixmapDescList descImagesList;
    QStringList imageFilesList;
    QStringListModel *model;

    QSlider *zoomSlider;

    QString getDescPage(const QString &f_name) const;


};

#endif // PREVIEWPAGE_H
