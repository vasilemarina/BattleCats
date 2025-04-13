#ifndef FINISHGAMEWINDOW_H
#define FINISHGAMEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTableWidget>

class FinishGameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FinishGameWindow(QWidget* parent = nullptr);

    void setGameStats(const QString& result, const QVector<QPair<QString, int>>& stats);

protected:
    void paintEvent(QPaintEvent* event) override;

signals:
    void returnToMainMenu();

private:
    QLabel* resultLabel;
    QTableWidget* statsTable;
    QPushButton* backToMenuButton;

    void setupUI();
};

#endif // FINISHGAMEWINDOW_H
