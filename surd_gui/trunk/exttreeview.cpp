#include "exttreeview.h"
#include <QMenu>
#include <QAction>

ExtTreeView::ExtTreeView(QWidget *parent)
    :QTreeView(parent)
{
    editAct = new QAction(QObject::trUtf8("Редактировать..."),this);
    clearAct = new QAction(QObject::trUtf8("Очистить..."),this);
}

void ExtTreeView::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu(this);
  menu.addAction(editAct);
  menu.addAction(clearAct);

  menu.exec(event->globalPos());
}

ExtTreeView::~ExtTreeView()
{

}
