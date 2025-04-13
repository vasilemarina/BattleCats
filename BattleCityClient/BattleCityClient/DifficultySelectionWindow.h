#ifndef DIFFICULTYSELECTIONWINDOW_H
#define DIFFICULTYSELECTIONWINDOW_H

#include "ui_DifficultySelectionWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <cpr/cpr.h>
#include <iostream>
#include <QMainWindow>
#include "Difficulty.h"
#include "Client.h"

namespace Ui {
class DifficultySelectionWindow;
}

class DifficultySelectionWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DifficultySelectionWindow(Client* client, QWidget* parent = nullptr);
    ~DifficultySelectionWindow();

    double getBulletSpeed() const;
    int getNumberOfBombs() const;
    int getNumberOfLives() const;
    int getExplosionRadius() const;
    int getBombDamage() const;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void closeEvent(QCloseEvent* event) override;

signals:
    void difficultySelected(Difficulty difficulty);
    void windowClosed();

private slots:
    void updateCustomSettingsVisibility();
    void on_btnStartGame_clicked();
    void on_btnCancel_clicked();

private:
    Ui::DifficultySelectionWindow *ui;
    Client* m_client; 
    double bulletSpeed;
    int numberOfBombs;
    int numberOfLives;
    int explosionRadius;
    int bombDamage;

};

#endif // DIFFICULTYSELECTIONWINDOW_H
