#pragma once

#include <QMainWindow>
#include <vector>
#include "Client.h"
#include "CreateAccountDialog.h"
#include "LoginDialog.h"
#include "MapWidget.h"
#include "FinishGameWindow.h"
#include "GameWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent* event) override;
private slots:
    void on_btnLogin_clicked();
    void on_btnCreateAccount_clicked();
    void onGameFinished(const QVector<QPair<QString, int>>& stats);
    
private:
    Ui::MainWindow* ui; 
    Client* m_client;   
    std::string m_currentUsername;
   
};
