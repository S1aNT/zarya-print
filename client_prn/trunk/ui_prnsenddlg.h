/********************************************************************************
** Form generated from reading UI file 'prnsenddlg.ui'
**
** Created: Fri Dec 9 02:36:08 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRNSENDDLG_H
#define UI_PRNSENDDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_PrnSendDlg
{
public:
    QVBoxLayout *verticalLayout;
    QProgressBar *progressBar;
    QFrame *line;
    QPlainTextEdit *plainTextEdit;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QPushButton *clearButton;
    QPushButton *dirtyButton;

    void setupUi(QDialog *PrnSendDlg)
    {
        if (PrnSendDlg->objectName().isEmpty())
            PrnSendDlg->setObjectName(QString::fromUtf8("PrnSendDlg"));
        PrnSendDlg->resize(640, 480);
        verticalLayout = new QVBoxLayout(PrnSendDlg);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        progressBar = new QProgressBar(PrnSendDlg);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setMinimum(1);
        progressBar->setMaximum(5);
        progressBar->setValue(0);

        verticalLayout->addWidget(progressBar);

        line = new QFrame(PrnSendDlg);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        plainTextEdit = new QPlainTextEdit(PrnSendDlg);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));

        verticalLayout->addWidget(plainTextEdit);

        frame = new QFrame(PrnSendDlg);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMinimumSize(QSize(0, 48));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        clearButton = new QPushButton(frame);
        clearButton->setObjectName(QString::fromUtf8("clearButton"));
        clearButton->setEnabled(false);
        clearButton->setDefault(true);

        horizontalLayout->addWidget(clearButton);

        dirtyButton = new QPushButton(frame);
        dirtyButton->setObjectName(QString::fromUtf8("dirtyButton"));
        dirtyButton->setEnabled(false);

        horizontalLayout->addWidget(dirtyButton);


        verticalLayout->addWidget(frame);


        retranslateUi(PrnSendDlg);

        QMetaObject::connectSlotsByName(PrnSendDlg);
    } // setupUi

    void retranslateUi(QDialog *PrnSendDlg)
    {
        PrnSendDlg->setWindowTitle(QApplication::translate("PrnSendDlg", "\320\237\320\265\321\207\320\260\321\202\321\214 \321\202\320\265\320\272\321\203\321\211\320\265\320\263\320\276 \320\264\320\276\320\272\321\203\320\274\320\265\320\275\321\202\320\260", 0, QApplication::UnicodeUTF8));
        clearButton->setText(QApplication::translate("PrnSendDlg", "\320\247\320\270\321\201\321\202\320\276\320\262\320\270\320\272", 0, QApplication::UnicodeUTF8));
        dirtyButton->setText(QApplication::translate("PrnSendDlg", "\320\221\320\240\320\220\320\232", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PrnSendDlg: public Ui_PrnSendDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRNSENDDLG_H
