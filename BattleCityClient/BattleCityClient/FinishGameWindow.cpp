#include "FinishGameWindow.h"
#include <QHeaderView>
#include <QPainter>

FinishGameWindow::FinishGameWindow(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void FinishGameWindow::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    resultLabel = new QLabel("Rezultat: ", this);
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    mainLayout->addWidget(resultLabel);


    statsTable = new QTableWidget(this);
    statsTable->setColumnCount(2);
    statsTable->setHorizontalHeaderLabels({ "Statistica", "Valoare" });
    statsTable->horizontalHeader()->setStretchLastSection(true);
    statsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    statsTable->setSelectionMode(QAbstractItemView::NoSelection);
    mainLayout->addWidget(statsTable);


    backToMenuButton = new QPushButton("Înapoi la Meniu", this);
    connect(backToMenuButton, &QPushButton::clicked, this, &FinishGameWindow::returnToMainMenu);
    mainLayout->addWidget(backToMenuButton);

    setLayout(mainLayout);
}

void FinishGameWindow::setGameStats(const QString& result, const QVector<QPair<QString, int>>& stats)
{
    resultLabel->setText(result);

    statsTable->setRowCount(stats.size());
    for (int i = 0; i < stats.size(); ++i) {
        statsTable->setItem(i, 0, new QTableWidgetItem(stats[i].first));
        statsTable->setItem(i, 1, new QTableWidgetItem(QString::number(stats[i].second)));
    }
}

void FinishGameWindow::paintEvent(QPaintEvent* event)
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