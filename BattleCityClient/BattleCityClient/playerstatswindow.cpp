#include "PlayerStatsWindow.h"
#include "ui_PlayerStatsWindow.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <cpr/cpr.h>


PlayerStatsWindow::PlayerStatsWindow(Client* client, QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::PlayerStatsWindow),
    m_client(client)
{
    ui->setupUi(this);
    this->setFixedSize(400, 200);
}


PlayerStatsWindow::~PlayerStatsWindow()
{
    delete ui;
}

void PlayerStatsWindow::setPlayerStats(const QString& nickname, int score, double bulletSpeed, double reloadTime)
{
    ui->usernameLabel->setText("Nickname: " + nickname);
    ui->scoreLabel->setText("Score: " + QString::number(score));
    ui->bulletSpeedLabel->setText("Bullet Speed: " + QString::number(bulletSpeed) + " units/sec");
    ui->reloadTimeLabel->setText("Reload Time: " + QString::number(reloadTime) + " sec");
}

void PlayerStatsWindow::on_btnUpgradeReloadTime_clicked() {
    if (m_client == nullptr) {
        QMessageBox::warning(this, "Error", "Client not initialized.");
        return;
    }

    QJsonObject payload;
    payload["username"] = QString::fromStdString(m_client->GetUsername());
    QJsonDocument doc(payload);

    cpr::Response response = cpr::Post(
        cpr::Url{ m_client->GetServerUrl() + "/upgrade_weapon" },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ doc.toJson().toStdString() }
    );

    if (response.status_code == 200) {
        QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(response.text).toUtf8());
        QJsonObject jsonObject = jsonResponse.object();

        QMessageBox::information(this, "Success", "Reload time upgraded successfully!");

        double newReloadTime = jsonObject["reload_time"].toDouble();
        ui->reloadTimeLabel->setText("Reload Time: " + QString::number(newReloadTime) + " sec");

        emit statsUpdated();
    }
    else {
        QJsonDocument responseDoc = QJsonDocument::fromJson(QString::fromStdString(response.text).toUtf8());
        QJsonObject responseObject = responseDoc.object();
        QString message = responseObject["message"].toString();
        QMessageBox::warning(this, "Error", "Failed to upgrade reload time: " + message);
    }
}

void PlayerStatsWindow::on_btnClose_clicked()
{
    this->close(); 
}
