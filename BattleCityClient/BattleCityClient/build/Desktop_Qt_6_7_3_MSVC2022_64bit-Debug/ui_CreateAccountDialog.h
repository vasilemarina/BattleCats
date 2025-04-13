/********************************************************************************
** Form generated from reading UI file 'CreateAccountDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEACCOUNTDIALOG_H
#define UI_CREATEACCOUNTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

QT_BEGIN_NAMESPACE

class Ui_CreateAccountDialog
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *CreateAccountDialog)
    {
        if (CreateAccountDialog->objectName().isEmpty())
            CreateAccountDialog->setObjectName("CreateAccountDialog");
        CreateAccountDialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(CreateAccountDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        retranslateUi(CreateAccountDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, CreateAccountDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, CreateAccountDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(CreateAccountDialog);
    } // setupUi

    void retranslateUi(QDialog *CreateAccountDialog)
    {
        CreateAccountDialog->setWindowTitle(QCoreApplication::translate("CreateAccountDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CreateAccountDialog: public Ui_CreateAccountDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEACCOUNTDIALOG_H
