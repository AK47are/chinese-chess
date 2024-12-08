#include <qthread.h>
#include <vector>
#include <Qtimer>
#include <QGraphicsScene>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->submitValue, &QPushButton::clicked, this, &MainWindow::trigger);
}

MainWindow::~MainWindow()
{
    delete ui;
}

using namespace std;

void MainWindow::trigger() {
    const int SCENE_WIDTH = 512;
    int k, x, y;
    k = ui->setK->value();
    x = ui->setX->value();
    y = ui->setY->value();
    int size = pow(2, k);
    int cell_size = SCENE_WIDTH / size - 1;
    vector<vector<bool>> chess(size, vector<bool>(size));
    vector<vector<QGraphicsRectItem*>> cell(size, vector<QGraphicsRectItem*>(size));
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->displayChess->setScene(scene);
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            QRectF rect(col * cell_size, row * cell_size, cell_size, cell_size);
            cell[row][col] = scene->addRect(rect, QPen(Qt::white), QBrush(Qt::black)); // 保存格子的指针以便动态更新
        }
    }
    solve(chess, cell, size, {0, 0}, {x, y});
    delete scene;
}

// 显示
// void MainWindow::display(vector<vector<bool>>& chess, vector<vector<QGraphicsRectItem*>>& cell) {
//     for (int i = 0; i < chess.size(); ++i) {
//         for (int j = 0; j < chess[i].size(); ++j) {
//             if (chess[i][j] == 1) cell[i][j]->setBrush(QBrush(Qt::red));
//         }
//     }
//     QCoreApplication::processEvents();
//     QThread::msleep(10);
// }

int MainWindow::check(int N, pair<int, int>& coord, pair<int, int>& block) {
    enum { LEFT_TOP, RIGHT_TOP, LEFT_BOTTOM, RIGHT_BOTTOM };
    if (block.first - coord.first < N / 2) {
        if (block.second - coord.second < N / 2)
            return LEFT_TOP;
        else
            return RIGHT_TOP;
    } else {
        if (block.second - coord.second < N / 2)
            return LEFT_BOTTOM;
        else
            return RIGHT_BOTTOM;
    }
}

// 覆盖
void MainWindow::cover(vector<vector<bool>>& chess, vector<vector<QGraphicsRectItem*>>& cell, int size, int pos, pair<int, int> coord,
           pair<int, int> blk) {
    int block_dx[4] = {size / 2 - 1, size / 2 - 1, size / 2, size / 2};
    int block_dy[4] = {size / 2 - 1, size / 2, size / 2 - 1, size / 2};
    if (chess[blk.first][blk.second] != 1) chess[blk.first][blk.second] = 1;
    cell[blk.first][blk.second]->setBrush(QBrush(Qt::red));
    for (int i = 0; i < 4; ++i) {
        if (i == pos) continue;
        if (chess[coord.first + block_dx[i]][coord.second + block_dy[i]] != 1) {
            chess[coord.first + block_dx[i]][coord.second + block_dy[i]] = 1;
            cell[coord.first + block_dx[i]][coord.second + block_dy[i]]->setBrush(QBrush(Qt::red));
        }
    }
    QCoreApplication::processEvents();
    QThread::msleep(1);
    // display(chess, cell);
}

// 分块
void MainWindow::block(vector<vector<bool>>& chess, vector<vector<QGraphicsRectItem*>>& cell, int size, int pos, pair<int, int> coord,
           pair<int, int> blk) {
    int coord_dx[4] = {0, 0, size / 2, size / 2};
    int coord_dy[4] = {0, size / 2, 0, size / 2};
    int block_dx[4] = {size / 2 - 1, size / 2 - 1, size / 2, size / 2};
    int block_dy[4] = {size / 2 - 1, size / 2, size / 2 - 1, size / 2};
    solve(chess, cell, size / 2,
          {coord.first + coord_dx[pos], coord.second + coord_dy[pos]},
          {blk.first, blk.second});
    for (int i = 0; i < 4; ++i) {
        if (i == pos) continue;
        solve(chess, cell, size / 2,
              {coord.first + coord_dx[i], coord.second + coord_dy[i]},
              {coord.first + block_dx[i], coord.second + block_dy[i]});
    }
}

void MainWindow::solve(vector<vector<bool>>& chess, vector<vector<QGraphicsRectItem*>>& cell, int size, pair<int, int> coord,
           pair<int, int> blk) {
    if (size == 1) return;
    int pos = check(size, coord, blk);
    cover(chess, cell, size, pos, coord, blk);
    block(chess, cell, size, pos, coord, blk);
}


