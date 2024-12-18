#include "chessboard.h"
#include "chesspieces.h"
#include <QGraphicsScene>
#include <QTcpSocket>
#include <QGraphicsTextItem>
#include <QPen>
#include <QFont>
#include <QMessageBox>

SimpleChessBoard::SimpleChessBoard(QGraphicsView *board) : WIDTH(board->width() / COLS),
    HEIGHT(board->height() / ROWS) {

    // 图像位置与逻辑位置映射。注意 x, y 轴与 Qt 的 x, y 轴相反
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            points[i][j] = QPointF((j + 1) * WIDTH, (i + 1) * HEIGHT);
            pieces[i][j] = nullptr;
        }
    }
    setView(board);
    drawBoard();
    initPieces();
}

void SimpleChessBoard::handlePieceNotice(AbstractChessPiece *piece) {
    if (piece == nullptr || winner != Winner::none) return;
    if (sel_piece != nullptr && sel_piece != piece) {
        movePiece(piece);
    } else {
        selectPiece(piece);
    }
}

void SimpleChessBoard::selectPiece(AbstractChessPiece *piece) {
     // 选择逻辑
    if (piece == nullptr) return;
    if (piece->getCamp() != curr_camp || piece->getCamp() == Camp::none) return;
    if (sel_piece == piece)
        setSelPie(nullptr);
    else
        setSelPie(piece);
}

void SimpleChessBoard::movePiece(AbstractChessPiece *target) {
    // 移动逻辑
    if (sel_piece == nullptr || target == nullptr) return;
    if (sel_piece->isAbleMove(target->getCoord()) &&
            sel_piece->getCamp() == curr_camp) {
        curr_camp = (curr_camp == Camp::black) ? Camp::red : Camp::black;
        QPoint tmp1 = sel_piece->getCoord();
        QPoint tmp2 = target->getCoord();
        sel_piece->setCoord(tmp2);
        target->setCoord(tmp1);
        killPiece(target);
    }
    setSelPie(nullptr);
}

void SimpleChessBoard::killPiece(AbstractChessPiece *target) {
    if (target->getCamp() == Camp::none) return;
    // HACK: 不优雅，不安全
    if (target->getName()[0] == "将") {
        winner = (target->getCamp() == Camp::black) ? Winner::red : Winner::black;
        // qDebug() << "出现赢家";
    }
    QPoint coord = target->getCoord();
    this->removeItem(target);
    // FIX: 莫名出现 QGraphicsItem::ungrabMouse: not a mouse grabber 警告。
    delete target;
    new NoPiece(*this, coord);
}

QPointF SimpleChessBoard::getPoint(QPoint coord) const {
    return points[coord.x()][coord.y()];
}

AbstractChessPiece *SimpleChessBoard::getSelPie() const {
    return sel_piece;
}

void SimpleChessBoard::setSelPie(AbstractChessPiece *piece) {
    auto tmp = sel_piece;
    sel_piece = piece;
    if (tmp) tmp->update();
    if (sel_piece) sel_piece->update();
}

AbstractChessPiece *SimpleChessBoard::getPiece(QPoint coord) const {
    return pieces[coord.x()][coord.y()];
}

void SimpleChessBoard::setPiece(QPoint coord, AbstractChessPiece *piece) {
    pieces[coord.x()][coord.y()] = piece;
}

Camp SimpleChessBoard::getCurrCamp() const {
    return curr_camp;
}

SimpleChessBoard::Winner SimpleChessBoard::getWinner() const {
    return winner;
}

void SimpleChessBoard::setView(QGraphicsView *board) {
    // 画布棋盘属性设置
    board->setScene(this);
    board->setBackgroundBrush(Qt::white);
    board->setRenderHint(QPainter::Antialiasing, true); // 抗锯齿
    board->setSceneRect(QRectF(points[0][0], points[ROWS - 1][COLS - 1])); // 锁死视角
}

void SimpleChessBoard::drawBoard() {
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

void SimpleChessBoard::initPieces()  {
    // 初始化状态变量
    winner = Winner::none;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (pieces[i][j] != nullptr) {
                this->removeItem(pieces[i][j]);
                delete pieces[i][j];
                pieces[i][j] = nullptr;
            }
        }
    }
    curr_camp = Camp::black;
    setSelPie(nullptr);


    // 会自动设置 piece[][] 和 addItem(...)
    General *tmp1 = new General(*this, Camp::red, {0, 4});
    General *tmp2 = new General(*this, Camp::black, {ROWS - 1, 4});
    tmp1->setRival(tmp2);
    tmp2->setRival(tmp1);
    new Guard(*this, Camp::red, {0, 3});
    new Guard(*this, Camp::red, {0, COLS - 4});
    new Guard(*this, Camp::black, {ROWS - 1, 3});
    new Guard(*this, Camp::black, {ROWS - 1, COLS - 4});
    new Elephant(*this, Camp::red, {0, 2});
    new Elephant(*this, Camp::red, {0, COLS - 3});
    new Elephant(*this, Camp::black, {ROWS - 1, 2});
    new Elephant(*this, Camp::black, {ROWS - 1, COLS - 3});
    new Horse(*this, Camp::red, {0, 1});
    new Horse(*this, Camp::red, {0, COLS - 2});
    new Horse(*this, Camp::black, {ROWS - 1, 1});
    new Horse(*this, Camp::black, {ROWS - 1, COLS - 2});
    new Chariot(*this, Camp::red, {0, 0});
    new Chariot(*this, Camp::red, {0, COLS - 1});
    new Chariot(*this, Camp::black, {ROWS - 1, 0});
    new Chariot(*this, Camp::black, {ROWS - 1, COLS - 1});
    new Cannon(*this, Camp::red, {2, 1});
    new Cannon(*this, Camp::red, {2, COLS - 2});
    new Cannon(*this, Camp::black, {ROWS - 3, 1});
    new Cannon(*this, Camp::black, {ROWS - 3, COLS - 2});
    new Solider(*this, Camp::red, {3, 0});
    new Solider(*this, Camp::red, {3, 2});
    new Solider(*this, Camp::red, {3, 4});
    new Solider(*this, Camp::red, {3, 6});
    new Solider(*this, Camp::red, {3, 8});
    new Solider(*this, Camp::black, {ROWS - 4, 0});
    new Solider(*this, Camp::black, {ROWS - 4, 2});
    new Solider(*this, Camp::black, {ROWS - 4, 4});
    new Solider(*this, Camp::black, {ROWS - 4, 6});
    new Solider(*this, Camp::black, {ROWS - 4, 8});

    addOtherPiece();

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (pieces[i][j] == nullptr)
                pieces[i][j] = new NoPiece(*this, {i, j});
        }
    }
}


IntActChessBoard::IntActChessBoard(QGraphicsView *board, QLabel *show_text,
       QPushButton *reset) : SimpleChessBoard(board),
        text_ui(show_text) {
    connect(reset, &QPushButton::clicked, this, &IntActChessBoard::initPieces);
    showText();
}

void IntActChessBoard::handlePieceNotice(AbstractChessPiece *piece) {
    SimpleChessBoard::handlePieceNotice(piece);
    showText();
}

void IntActChessBoard::initPieces() {
    SimpleChessBoard::initPieces();
    showText();
}

void IntActChessBoard::showText() {
    text_ui->setFont(QFont("fangsong", 20, Qt::yellow));
    text_ui->setAlignment(Qt::AlignCenter);
    if (getWinner() == Winner::none) {
        if (getCurrCamp() == Camp::black)
            text_ui->setText("轮到黑棋出棋");
        else if (getCurrCamp() == Camp::red)
            text_ui->setText("轮到红棋出棋");
        else
            text_ui->setText("错误");
    } else {
        if (getCurrCamp() == Camp::black)
            text_ui->setText("黑棋胜利");
        else if (getCurrCamp() == Camp::red)
            text_ui->setText("红棋胜利");
        else
            text_ui->setText("错误");
    }
}

NetChessBoard::NetChessBoard(QGraphicsView *board, Camp cp, QHostAddress ip,
                    quint16 port) : SimpleChessBoard(board), camp(cp) {
    if (camp == Camp::black) {
        black = new QTcpServer(this);
        black->listen(ip, port);
        connect(black, &QTcpServer::newConnection, this,
                &NetChessBoard::onNewConnection);
        QMessageBox::information(nullptr, "提示", "服务器创建成功");
    } else if (camp == Camp::red) {
        red = new QTcpSocket();
        red->connectToHost(ip, port);
        connect(red, &QTcpSocket::readyRead, this, &NetChessBoard::onClientReadyRead);
    } else {
        QMessageBox::information(nullptr, "警告", "阵营不存在");
    }
}

void NetChessBoard::handlePieceNotice(AbstractChessPiece *piece) {
    if (red == nullptr) {
        QMessageBox::information(nullptr, "警告", "当前玩家人数不足");
        return;
    }
    if (camp == getCurrCamp()) {
        auto prev_sel = getSelPie();
        SimpleChessBoard::handlePieceNotice(piece);
        if (camp != getCurrCamp()) { // 代表有移动。
            QByteArray data;
            QDataStream out(&data, QIODevice::WriteOnly);
            out << prev_sel->getCoord() << piece->getCoord();  // 将 QPoint 对象写入到数据流中
            red->write(data);
        }
    }
}

// 只能给 black 绑定这个槽函数。
void NetChessBoard::onNewConnection() {
    if (red == nullptr) {
        red = black->nextPendingConnection();
        QMessageBox::information(nullptr, "提示", "有玩家进入房间");
        connect(red, &QTcpSocket::readyRead, this, &NetChessBoard::onClientReadyRead);
        connect(red, &QTcpSocket::disconnected, this, &NetChessBoard::onDisconnected);
    } else {
        QMessageBox::information(nullptr, "警告", "已经有玩家进入房间");
    }
}

void NetChessBoard::onClientReadyRead() {
    QByteArray data = red->readAll();
    QDataStream in(&data, QIODevice::ReadOnly);
    QPoint start, end;
    in >> end >> start;
    // qDebug() << start << " " << end;
    setSelPie(getPiece(start));
    movePiece(getPiece(end));
}

void NetChessBoard::onDisconnected() {
    red = nullptr;
    QMessageBox::information(nullptr, "警告", "有玩家断开连接");
}
