#include "DifficultySelectionWindow.h"
#include "ui_DifficultySelectionWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QPainter>

DifficultySelectionWindow::DifficultySelectionWindow(Client* client, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::DifficultySelectionWindow)
    , m_client(client) 
    , bulletSpeed(0.05)
    , numberOfBombs(1)
    , numberOfLives(1)
    , explosionRadius(1)
    , bombDamage(5)
{
    ui->setupUi(this);
    this->setFixedSize(800, 600);


    updateCustomSettingsVisibility();

    connect(ui->radioButtonCustom, &QRadioButton::toggled, this, &DifficultySelectionWindow::updateCustomSettingsVisibility);
    connect(ui->radioButtonEasy, &QRadioButton::toggled, this, &DifficultySelectionWindow::updateCustomSettingsVisibility);
    connect(ui->radioButtonMedium, &QRadioButton::toggled, this, &DifficultySelectionWindow::updateCustomSettingsVisibility);
    connect(ui->radioButtonHard, &QRadioButton::toggled, this, &DifficultySelectionWindow::updateCustomSettingsVisibility);

    connect(ui->comboBox, &QComboBox::currentTextChanged, [this](const QString& value) {
        bulletSpeed = value.toDouble();
        });

    connect(ui->comboBox_2, &QComboBox::currentTextChanged, [this](const QString& value) {
        numberOfBombs = value.toInt();
        });

    connect(ui->comboBox_3, &QComboBox::currentTextChanged, [this](const QString& value) {
        numberOfLives = value.toInt();
        });

    connect(ui->comboBox_4, &QComboBox::currentTextChanged, [this](const QString& value) {
        explosionRadius = value.toInt();
        });

    connect(ui->comboBox_5, &QComboBox::currentTextChanged, [this](const QString& value) {
        bombDamage = value.toInt();
        });
}

DifficultySelectionWindow::~DifficultySelectionWindow()
{
    delete ui;
}

double DifficultySelectionWindow::getBulletSpeed() const
{
    return bulletSpeed;
}

int DifficultySelectionWindow::getNumberOfBombs() const
{
    return numberOfBombs;
}

int DifficultySelectionWindow::getNumberOfLives() const
{
    return numberOfLives;
}

int DifficultySelectionWindow::getExplosionRadius() const
{
    return explosionRadius;
}

int DifficultySelectionWindow::getBombDamage() const
{
    return bombDamage;
}


void DifficultySelectionWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QImage backgroundImage(":/resources/resources/background3.jpg");
    if (backgroundImage.isNull()) {
        qDebug() << "Nu s-a putut incarca imaginea!";
    }
    else {
        QPixmap background = QPixmap::fromImage(backgroundImage);
        painter.drawPixmap(0, 0, width(), height(), background);
    }
}

void DifficultySelectionWindow::closeEvent(QCloseEvent* event)
{
    if (!event->spontaneous())
        return;

    event->accept();
    std::string errorMessage;
    if (!m_client->Logout(m_client->GetUsername(), errorMessage)) {
        qDebug() << "Failed to notify server about logout: " << QString::fromStdString(errorMessage);
    }

    QApplication::quit();
}

void DifficultySelectionWindow::updateCustomSettingsVisibility()
{
    bool isCustom = ui->radioButtonCustom->isChecked();

    ui->label->setVisible(isCustom);
    ui->label_2->setVisible(isCustom);
    ui->label_3->setVisible(isCustom);
    ui->label_4->setVisible(isCustom);
    ui->label_5->setVisible(isCustom);

    ui->comboBox->setVisible(isCustom);
    ui->comboBox_2->setVisible(isCustom);
    ui->comboBox_3->setVisible(isCustom);
    ui->comboBox_4->setVisible(isCustom);
    ui->comboBox_5->setVisible(isCustom);
}


void DifficultySelectionWindow::on_btnStartGame_clicked()
{
    Difficulty selectedDifficulty; 

    if (ui->radioButtonEasy->isChecked()) {
        selectedDifficulty = Difficulty::EASY;
    }
    else if (ui->radioButtonMedium->isChecked()) {
        selectedDifficulty = Difficulty::MEDIUM;
    }
    else if (ui->radioButtonHard->isChecked()) {
        selectedDifficulty = Difficulty::HARD;
    }
    else if (ui->radioButtonCustom->isChecked()) {
        selectedDifficulty = Difficulty::CUSTOM;
    }
    else {
        QMessageBox::warning(this, "Warning", "Please select a difficulty level.");
        return;
    }

    QJsonObject json;
    json["difficulty"] = ui->radioButtonCustom->isChecked() ? "CUSTOM" :
        ui->radioButtonEasy->isChecked() ? "EASY" :
        ui->radioButtonMedium->isChecked() ? "MEDIUM" : "HARD";

    if (ui->radioButtonCustom->isChecked()) {
        json["bullet_speed"] = getBulletSpeed();
        json["custom_bombs"] = getNumberOfBombs();
        json["lives"] = getNumberOfLives();
        json["explosion_radius"] = getExplosionRadius();
        json["bomb_damage"] = getBombDamage();
    }

    QJsonDocument doc(json);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    if (m_client) {
        m_client->sendGameSettings(jsonString.toStdString());
    }
    else {
        QMessageBox::warning(this, "Error", "Client is not initialized!");
    }

	emit difficultySelected(selectedDifficulty); 
    close();
}


void DifficultySelectionWindow::on_btnCancel_clicked()
{
    close();
    emit windowClosed();
}
