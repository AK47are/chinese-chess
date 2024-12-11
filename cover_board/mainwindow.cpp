#include "mainwindow.h"

#include <QGraphicsScene>
#include <QThread>
#include <Qtimer>

#include "./ui_mainwindow.h"
#include "solvechess.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  scene = new QGraphicsScene(this);
  ui->displayChess->setScene(scene);
  connect(ui->submitValue, &QPushButton::clicked, this, &MainWindow::trigger);
}

MainWindow::~MainWindow() {
  for (int i = 0; i < cell.size(); ++i) {
    for (int j = 0; j < cell.size(); ++j) {
      delete cell[i][j];
    }
  }
  delete scene;
  delete ui;
}

void MainWindow::trigger() {
  const int SCENE_WIDTH = 512;
  int k, x, y;
  k = ui->setK->value();
  x = ui->setX->value();
  y = ui->setY->value();
  draw_count = 0;
  int size = pow(2, k);
  chess = new SolveChess(size, {x, y});
  int cell_size = SCENE_WIDTH / size - 1;
  for (int i = 0; i < cell.size(); ++i) {
    for (int j = 0; j < cell.size(); ++j) {
      delete cell[i][j];
    }
  }
  cell.resize(size, vector<QGraphicsRectItem *>(size));
  for (int row = 0; row < size; ++row) {
    for (int col = 0; col < size; ++col) {
      QRectF rect(col * cell_size, row * cell_size, cell_size, cell_size);
      cell[row][col] =
          scene->addRect(rect, QPen(Qt::white),
                         QBrush(Qt::black));  // 保存格子的指针以便动态更新
    }
  }
  on_showContinue_clicked();
}

void MainWindow::on_showStop_clicked() {
  is_stop = true;
  ui->showStop->setEnabled(false);
  ui->showContinue->setEnabled(true);
}

void MainWindow::on_showContinue_clicked() {
  is_stop = false;
  ui->showStop->setEnabled(true);
  ui->showContinue->setEnabled(false);
  while (draw_count < chess->change.size()) {
    for (int j = 0; j < 3; ++j) {
      cell[chess->change[draw_count][j].first]
          [chess->change[draw_count][j].second]
              ->setBrush(QBrush(Qt::red));
    }
    QCoreApplication::processEvents();
    QThread::msleep(1);
    ++draw_count;
    if (is_stop) break;
  }
}

void MainWindow::on_showBack_clicked() {
  // qDebug() << is_stop << " " << draw_count;
  if (is_stop && draw_count >= 0) {
    if (draw_count != 0) --draw_count;
    for (int j = 0; j < 3; ++j) {
      cell[chess->change[draw_count][j].first]
          [chess->change[draw_count][j].second]
              ->setBrush(QBrush(Qt::black));
    }
    QCoreApplication::processEvents();
    // qDebug() << "Back!";
  }
}

void MainWindow::on_showForward_clicked() {
  // qDebug() << is_stop << " " << draw_count;
  if (is_stop && draw_count < chess->change.size()) {
    for (int j = 0; j < 3; ++j) {
      cell[chess->change[draw_count][j].first]
          [chess->change[draw_count][j].second]
              ->setBrush(QBrush(Qt::red));
    }
    QCoreApplication::processEvents();
    ++draw_count;
    // qDebug() << "Forward!";
  }
}
