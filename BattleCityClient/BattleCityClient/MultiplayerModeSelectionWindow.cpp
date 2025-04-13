
#include "MultiplayerModeSelectionWindow.h"
#include "ui_MultiplayerModeSelectionWindow.h"
#include <QMessageBox>
#include <QDebug>
#include "GameMenuWindow.h"
#include "DifficultySelectionWindow.h"
#include "GameWindow.h"
#include <QTimer>
#include <QInputDialog>    
#include <QLineEdit>       

MultiplayerModeSelectionWindow::MultiplayerModeSelectionWindow(Client* client, GameMenuWindow* parentWindow, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MultiplayerModeSelectionWindow)
    , m_client(client)
    , m_parentWindow(parentWindow)
{
    ui->setupUi(this);
    this->setFixedSize(800, 600);
}

MultiplayerModeSelectionWindow::~MultiplayerModeSelectionWindow()
{
    delete ui;
}

void MultiplayerModeSelectionWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QImage backgroundImage(":/resources/resources/background3.jpg");
    if (backgroundImage.isNull()) {
        qDebug() << "Failed to load background image!";
    }
    else {
        QPixmap background = QPixmap::fromImage(backgroundImage);
        painter.drawPixmap(0, 0, width(), height(), background);
    }
}

void MultiplayerModeSelectionWindow::on_btnCreateGame_clicked()
{
    if (!m_client) {
        qDebug() << "Client not initialized!";
        return;
    }

    auto difficultyWindow = new DifficultySelectionWindow(m_client, this);
    connect(difficultyWindow, &DifficultySelectionWindow::difficultySelected, this, &MultiplayerModeSelectionWindow::handleDifficultyWindowClosed);
    difficultyWindow->setAttribute(Qt::WA_DeleteOnClose);
    difficultyWindow->show();

    this->hide();
}

void MultiplayerModeSelectionWindow::on_btnJoinGame_clicked()
{
    if (!m_client) {
        QMessageBox::warning(this, "Error", "Client is not initialized.");
        return;
    }

    std::string joinMessage;
    std::string errorMessage;
    if (m_client->JoinMultiplayerGame(joinMessage, errorMessage)) {
        QMessageBox::information(this, "Join Game", QString::fromStdString(joinMessage));

        handleGameStart();
    }
    else {
        QMessageBox::warning(this, "Join Game Failed", QString::fromStdString(errorMessage));
    }
}


void MultiplayerModeSelectionWindow::on_btnBack_clicked()
{
    emit windowClosed();
    m_parentWindow->show();
    this->close();
}

void MultiplayerModeSelectionWindow::handleDifficultyWindowClosed()
{
    QMessageBox::information(this, "Game Created", "Game created successfully. Waiting for another player to join.");

    ui->btnCreateGame->setEnabled(false);
    ui->btnJoinGame->setEnabled(false);

    QTimer* pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, [=]() {
        bool gameStarted = false;
        std::string message;
        std::string errorMessage;
        if (m_client->PollMultiplayerGameStatus(gameStarted, message, errorMessage)) {
            if (gameStarted) {
                pollTimer->stop();
                QMessageBox::information(this, "Game Started", "Another player has joined. The game is starting!");

                GameWindow* gameWindow = new GameWindow(m_client, m_client->GetUsername());
                gameWindow->setAttribute(Qt::WA_DeleteOnClose); 

                connect(gameWindow, &QObject::destroyed, this, &MultiplayerModeSelectionWindow::show);

                gameWindow->show();
                this->hide(); 
            }
        }
        else {
            qDebug() << "Polling game status failed:" << QString::fromStdString(errorMessage);
        }
        });

    pollTimer->start(6000);
}


void MultiplayerModeSelectionWindow::handleGameStart()
{
    GameWindow* gameWindow = new GameWindow(m_client, m_client->GetUsername());
    gameWindow->setAttribute(Qt::WA_DeleteOnClose); 

    connect(gameWindow, &QObject::destroyed, this, &MultiplayerModeSelectionWindow::show);

    gameWindow->show();
    this->hide();
}
