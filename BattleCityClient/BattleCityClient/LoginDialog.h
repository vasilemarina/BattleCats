#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog();

    QString GetUsername() const;
    QString GetPassword() const;

private slots:
    void onLoginButtonClicked();

private:
    Ui::LoginDialog* ui;
    QLineEdit* m_usernameLineEdit;    
    QLineEdit* m_passwordLineEdit;
    QPushButton* m_loginButton;
};