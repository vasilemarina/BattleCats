#include "mainWindow.h"
#include "ui_mainWindow.h" 
#include <QDebug>
#include <QPainter>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QKeyEvent>
#include "GameMenuWindow.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_client(new Client("http://localhost:18080")) {
    ui->setupUi(this);
	this->setFixedSize(800, 600);
    connect(m_client, &Client::gameFinished, this, &MainWindow::onGameFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QImage backgroundImage(":/resources/resources/background1.jpg");
    if (backgroundImage.isNull()) {
        qDebug() << "Nu s-a putut incarca imaginea!";
    }
    else {
        QPixmap background = QPixmap::fromImage(backgroundImage);
        painter.drawPixmap(0, 0, width(), height(), background);
    }
}

void MainWindow::on_btnLogin_clicked() {
    LoginDialog loginDialog(this);
    if (loginDialog.exec() == QDialog::Accepted) {
        QString username = loginDialog.GetUsername();
        QString password = loginDialog.GetPassword();
        std::string errorMessage;

        if (m_client->Login(username.toStdString(), password.toStdString(), errorMessage)) {
            m_currentUsername = username.toStdString();
            
            GameMenuWindow* menuWindow = new GameMenuWindow(m_client);
            menuWindow->setAttribute(Qt::WA_DeleteOnClose); 
            menuWindow->show();

            this->close();
        }
    }
}

void MainWindow::on_btnCreateAccount_clicked() {
    CreateAccountDialog createAccountDialog(this);
    if (createAccountDialog.exec() == QDialog::Accepted) {
        QString username = createAccountDialog.GetUsername();
        QString password = createAccountDialog.GetPassword();
        std::string errorMessage;

        if (m_client->Register(username.toStdString(), password.toStdString(), errorMessage)) {
            QMessageBox::information(this, "Success", "Account created successfully! Now you can login.");
        }
        else {
            QMessageBox::warning(this, "Error", QString::fromStdString(errorMessage));
        }
    }
}

void MainWindow::onGameFinished(const QVector<QPair<QString, int>>& stats) {
    FinishGameWindow* finishGameWindow = new FinishGameWindow(this);
    finishGameWindow->setGameStats("Game Over", stats);

    connect(finishGameWindow, &FinishGameWindow::returnToMainMenu, this, [this, finishGameWindow]() {
        finishGameWindow->close();
        finishGameWindow->deleteLater();

        GameMenuWindow* gameMenuWindow = new GameMenuWindow(m_client);
        gameMenuWindow->setAttribute(Qt::WA_DeleteOnClose);
        gameMenuWindow->show();
        });

    finishGameWindow->show();
}




