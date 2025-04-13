#include "LoginDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include "ui_LoginDialog.h"

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    setWindowTitle("Login");

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* usernameLabel = new QLabel("Username:", this);
    layout->addWidget(usernameLabel);

    m_usernameLineEdit = new QLineEdit(this);
    layout->addWidget(m_usernameLineEdit);

    QLabel* passwordLabel = new QLabel("Password:", this);
    layout->addWidget(passwordLabel);

    m_passwordLineEdit = new QLineEdit(this);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(m_passwordLineEdit);

    m_loginButton = new QPushButton("Login", this);
    layout->addWidget(m_loginButton);

    connect(m_loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginButtonClicked);

    setLayout(layout);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QString LoginDialog::GetUsername() const
{
    return m_usernameLineEdit->text();
}

QString LoginDialog::GetPassword() const
{
    return m_passwordLineEdit->text();
}

void LoginDialog::onLoginButtonClicked()
{
    accept();
}
