#ifndef PRNSENDDLG_H
#define PRNSENDDLG_H

#include <QtGui/QDialog>

namespace Ui {
    class PrnSendDlg;
}

class PrnSendDlg : public QDialog {
    Q_OBJECT
public:
    PrnSendDlg(QWidget *parent = 0);
    ~PrnSendDlg();
signals:
    void clearDoc();
    void dirtyDoc();

public slots:
    void showPrnState(const QString &info);
    void printNextCopy();
    void enableButtons();
    void disableButtons();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::PrnSendDlg *m_ui;
    int m_copy;
};

#endif // PRNSENDDLG_H
