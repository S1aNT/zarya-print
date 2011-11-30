#ifndef EXTTREEVIEW_H
#define EXTTREEVIEW_H

#include <QTreeView>

class QAction;

class ExtTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit ExtTreeView(QWidget *parent = 0);
    ~ExtTreeView();
signals:

public slots:
private:
    QAction *editAct;
    QAction *clearAct;
protected:
    void contextMenuEvent(QContextMenuEvent *event);

};


#endif // EXTTREEVIEW_H
