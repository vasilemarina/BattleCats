#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
    class CreateAccountDialog;
}

class CreateAccountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateAccountDialog(QWidget* parent = nullptr);
    ~CreateAccountDialog();

    QString GetUsername() const;
    QString GetPassword() const;

private slots:
    void onCreateButtonClicked();

private:
    Ui::CreateAccountDialog* ui;
    QLineEdit* m_usernameLineEdit;   
    QLineEdit* m_passwordLineEdit;
    QPushButton* m_createButton;
};