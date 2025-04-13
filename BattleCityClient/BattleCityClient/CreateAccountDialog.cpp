#include "CreateAccountDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <regex>
#include <QMessageBox>
#include "ui_CreateAccountDialog.h"

bool isValidUsername(const std::string& username) {
    std::regex usernameRegex("^[a-zA-Z0-9]{5,}$");
    return std::regex_match(username, usernameRegex);
}

bool isValidPassword(const std::string& password) {
    std::regex passwordRegex("^[A-Za-z\\d@$!%*?&]{4,}$");
    return std::regex_match(password, passwordRegex);
}

CreateAccountDialog::CreateAccountDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::CreateAccountDialog)
{
    setWindowTitle("Create Account");

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

    m_createButton = new QPushButton("Create Account", this);
    layout->addWidget(m_createButton);

    connect(m_createButton, &QPushButton::clicked, this, &CreateAccountDialog::onCreateButtonClicked);

    setLayout(layout);
}

CreateAccountDialog::~CreateAccountDialog()
{
    delete ui;
}

QString CreateAccountDialog::GetUsername() const
{
    return m_usernameLineEdit->text();
}

QString CreateAccountDialog::GetPassword() const
{
    return m_passwordLineEdit->text();
}

void CreateAccountDialog::onCreateButtonClicked()
{
    std::string username = m_usernameLineEdit->text().toStdString();
    std::string password = m_passwordLineEdit->text().toStdString();

    if (!isValidUsername(username)) {
        QMessageBox::warning(this, "Error", "The username must contain only letters, numbers, and have at least 5 characters.");
        return;
    }

    if (!isValidPassword(password)) {
        QMessageBox::warning(this, "Error", "The password must be at least 4 characters long and can contain letters, numbers, and special characters.");
        return;
    }

    accept();
}
