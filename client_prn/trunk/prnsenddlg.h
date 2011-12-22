#ifndef PRNSENDDLG_H
#define PRNSENDDLG_H

#include <QtGui/QDialog>
#include <QSignalMapper>

namespace Ui {
    class PrnSendDlg;
}

class PrnSendDlg : public QDialog {
    Q_OBJECT
public:
    PrnSendDlg(QWidget *parent = 0);
    ~PrnSendDlg();
    void checkNextCopy();

signals:
    void clearDoc();
    void dirtyDoc();
    void getNextCopy(int copy);

public slots:
    void showPrnState(const QString &info);  
    void do_needMarkDoc();
protected:
    void changeEvent(QEvent *e);
private slots:
    void do_clicked(int btn);
private:

    Ui::PrnSendDlg *m_ui;
    int m_copy;
    QSignalMapper *signalMapper;

    void setButtons(bool state);

};

#endif // PRNSENDDLG_H
