#include "chessboard.h"
#include "chesspieces.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPen>
#include <QFont>

ChessBoard::ChessBoard(QGraphicsView *board) : WIDTH(board->width() / COLS),
    HEIGHT(board->height() / ROWS) {

    // 图像位置与逻辑位置映射。注意 x, y 轴与 Qt 的 x, y 轴相反
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            points[i][j] = QPointF((j + 1) * WIDTH, (i + 1) * HEIGHT);
        }
    }

    drawBoard(board);
    initPieces();
}

void ChessBoard::judgeSelectOrMove(AbstractChessPiece *piece) {
    if (piece == nullptr) return;
    if (sel_piece != nullptr && sel_piece != piece) {
        movePiece(piece);
    } else {
        selectPiece(piece);
    }
}

void ChessBoard::selectPiece(AbstractChessPiece *piece) {
     // 选择逻辑
    if (piece == nullptr || piece->getCamp() == Camp::none) return;
    if (sel_piece == piece) {
        sel_piece = nullptr;
        piece->update();
    } else {
        sel_piece = piece;
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                pieces[i][j]->update();
            }
        }
    }
}

void ChessBoard::movePiece(AbstractChessPiece *target) {
    // 移动逻辑
    if (sel_piece == nullptr || target == nullptr) return;
    if (sel_piece->isAbleMove(target->getCoord())) {
        QPoint tmp1 = sel_piece->getCoord();
        QPoint tmp2 = target->getCoord();
        sel_piece->setCoord(tmp2);
        target->setCoord(tmp1);
        killPiece(target);
    } else {
        auto tmp = sel_piece;
        sel_piece = nullptr;
        tmp->update();
    }
}

void ChessBoard::killPiece(AbstractChessPiece *target) {
    if (target->getCamp() == Camp::none) return;
    QPoint coord = target->getCoord();
    this->removeItem(target);
    delete target; // HACK: 莫名出现 QGraphicsItem::ungrabMouse: not a mouse grabber 警告。
    new NoPiece(this, coord);
}

QPointF ChessBoard::getPoint(QPoint coord) {
    return points[coord.x()][coord.y()];
}

AbstractChessPiece *&ChessBoard::getPiece(QPoint coord) {
    return pieces[coord.x()][coord.y()];
}

void ChessBoard::drawBoard(QGraphicsView *board) {
    // 画布棋盘属性设置
    board->setScene(this);
    board->setBackgroundBrush(Qt::white);
    board->setRenderHint(QPainter::Antialiasing, true); // 抗锯齿
    board->setSceneRect(QRectF(points[0][0], points[ROWS - 1][COLS - 1])); // 锁死视角

    // 绘图
    addRect(QRectF(points[0][0], points[ROWS - 1][COLS - 1]));
    for (int i = 1; i < ROWS - 1; ++i) {
        addLine(QLineF(points[i][0], points[i][COLS - 1]));
    }
    for (int i = 1; i < COLS - 1; ++i) {
        addLine(QLineF(points[0][i], points[4][i]));
        addLine(QLineF(points[5][i], points[ROWS - 1][i]));
    }
    addLine(QLineF(points[0][3], points[2][5]));
    addLine(QLineF(points[0][5], points[2][3]));
    addLine(QLineF(points[ROWS - 3][3], points[ROWS - 1][5]));
    addLine(QLineF(points[ROWS - 3][5], points[ROWS - 1][3]));

    QGraphicsTextItem *chu = addText("楚河", QFont("Arial", 20));
    QGraphicsTextItem *han = addText("汉界", QFont("Arial", 20));
    chu->setPos(2 * WIDTH, 5.15 * HEIGHT);
    han->setPos(7 * WIDTH, 5.15 * HEIGHT);
    chu->setDefaultTextColor(Qt::black);
    han->setDefaultTextColor(Qt::black);
}

void ChessBoard::initPieces()  {
    // 会自动设置 piece[][] 和 addItem(...)
    General *tmp1 = new General(this, Camp::red, {0, 4});
    General *tmp2 = new General(this, Camp::black, {ROWS - 1, 4});
    tmp1->setRival(tmp2);
    tmp2->setRival(tmp1);
    new Guard(this, Camp::red, {0, 3});
    new Guard(this, Camp::red, {0, COLS - 4});
    new Guard(this, Camp::black, {ROWS - 1, 3});
    new Guard(this, Camp::black, {ROWS - 1, COLS - 4});
    new Elephant(this, Camp::red, {0, 2});
    new Elephant(this, Camp::red, {0, COLS - 3});
    new Elephant(this, Camp::black, {ROWS - 1, 2});
    new Elephant(this, Camp::black, {ROWS - 1, COLS - 3});
    new Horse(this, Camp::red, {0, 1});
    new Horse(this, Camp::red, {0, COLS - 2});
    new Horse(this, Camp::black, {ROWS - 1, 1});
    new Horse(this, Camp::black, {ROWS - 1, COLS - 2});
    new Chariot(this, Camp::red, {0, 0});
    new Chariot(this, Camp::red, {0, COLS - 1});
    new Chariot(this, Camp::black, {ROWS - 1, 0});
    new Chariot(this, Camp::black, {ROWS - 1, COLS - 1});
    new Cannon(this, Camp::red, {2, 1});
    new Cannon(this, Camp::red, {2, COLS - 2});
    new Cannon(this, Camp::black, {ROWS - 3, 1});
    new Cannon(this, Camp::black, {ROWS - 3, COLS - 2});
    new Solider(this, Camp::red, {3, 0});
    new Solider(this, Camp::red, {3, 2});
    new Solider(this, Camp::red, {3, 4});
    new Solider(this, Camp::red, {3, 6});
    new Solider(this, Camp::red, {3, 8});
    new Solider(this, Camp::black, {ROWS - 4, 0});
    new Solider(this, Camp::black, {ROWS - 4, 2});
    new Solider(this, Camp::black, {ROWS - 4, 4});
    new Solider(this, Camp::black, {ROWS - 4, 6});
    new Solider(this, Camp::black, {ROWS - 4, 8});

    addOtherPiece();

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (pieces[i][j] == nullptr)
                pieces[i][j] = new NoPiece(this, {i, j});
        }
    }
}

