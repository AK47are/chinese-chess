#include "chessboard.h"
#include "chesspieces.h"
#include <QGraphicsScene>
#include <QTcpSocket>
#include <QGraphicsTextItem>
#include <QPen>
#include <QFont>
#include <QMessageBox>
#include <QVBoxLayout>

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
        execMove(sel_piece->getCoord(), piece->getCoord());
    } else {
        execSelect(piece);
    }
}

void SimpleChessBoard::execSelect(AbstractChessPiece *piece) {
     // 选择逻辑
    if (piece == nullptr) return;
    if (winner != Winner::none || piece->getCamp() != curr_camp ||
        piece->getCamp() == Camp::none) return;
    if (sel_piece == piece)
        setSelPie(nullptr);
    else
        setSelPie(piece);
}

void SimpleChessBoard::execMove(QPoint start, QPoint end) {
    // 移动逻辑
    if (winner != Winner::none || getPiece(start) == nullptr ||
        getPiece(end) == nullptr) return;
    if (getPiece(start)->isAbleMove(end) &&
            getPiece(start)->getCamp() == curr_camp) {
        movePiece(start, end);
    }
    setSelPie(nullptr);
}

void SimpleChessBoard::execKill(AbstractChessPiece *target) {
    if (winner != Winner::none || target->getCamp() == Camp::none) return;
    if (target->getName()[0] == "将") {
        winner = (target->getCamp() == Camp::black) ? Winner::red : Winner::black;
        // qDebug() << "出现赢家";
    }
    QPoint coord = target->getCoord();
    this->removeItem(target);
    // FIXED: 莫名出现 QGraphicsItem::ungrabMouse: not a mouse grabber 警告。
    // 删掉棋子鼠标事件中的 QGraphicsItem::mousePressEvent(event); 就恢复了。
    delete target;
    new NoPiece(*this, coord);
}

void SimpleChessBoard::movePiece(QPoint start, QPoint end) {
    curr_camp = (curr_camp == Camp::black) ? Camp::red : Camp::black;
    auto start_piece = getPiece(start);
    auto end_piece = getPiece(end);
    start_piece->setCoord(end);
    end_piece->setCoord(start);
    execKill(end_piece);
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
       QPushButton *reset, QPushButton *back) : SimpleChessBoard(board),
        text_ui(show_text) {
    connect(reset, &QPushButton::clicked, this, &IntActChessBoard::initPieces);
    connect(back, &QPushButton::clicked, this, &IntActChessBoard::backMove);
    // 在构造函数中无法使用多态性。
    updateAll();
}

void IntActChessBoard::initPieces() {
    SimpleChessBoard::initPieces();
    if (is_back == false) {
        now_index = -1;
        change.clear();
    }
    updateAll();
}

void IntActChessBoard::movePiece(QPoint start, QPoint end) {
    SimpleChessBoard::movePiece(start, end);
    if (is_back == false && is_review == false) {
        ++now_index;
        if (now_index == change.size()) {
            change.append({start, end});
        } else {
            change[now_index] = {start, end};
        }
    }
    updateAll();
}

// 简单粗暴
bool IntActChessBoard::backMove() {
    if (change.size() == 0) return false;
    if (now_index < 0 || now_index > change.size()) return false;
    // 防止 NetChessBoard 重载后多次调用发送消息。
    is_back = true;
    --now_index;
    IntActChessBoard::initPieces();
    for (int i = 0; i <= now_index; ++i) {
        // 防止 NetChessBoard 重载后多次调用发送消息。
        IntActChessBoard::movePiece(change[i][0], change[i][1]);
    }
    is_back = false;
    return true;
}

bool IntActChessBoard::forwardMove() {
    if (now_index >= change.size() - 1) return false;
    ++now_index;
    IntActChessBoard::movePiece(change[now_index][0], change[now_index][1]);
    return true;
}

void IntActChessBoard::updateAll() {
    updateText();
    updateWinner();
}

void IntActChessBoard::updateText() {
    text_ui->setFont(QFont("fangsong", 20, Qt::yellow));
    text_ui->setAlignment(Qt::AlignCenter);
        if (getCurrCamp() == Camp::black)
            text_ui->setText("轮到黑棋出棋");
        else if (getCurrCamp() == Camp::red)
            text_ui->setText("轮到红棋出棋");
        else
            text_ui->setText("错误");
}

void IntActChessBoard::updateWinner() {
    if (is_review == true) return;
    if (getWinner() != Winner::none) {
        if (getWinner() == Winner::black) {
            QMessageBox::information(nullptr, "胜利", "黑棋胜利");
        } else if (getWinner() == Winner::red){
            QMessageBox::information(nullptr, "胜利", "红棋胜利");
        }
        queryReview();
    }
}

void IntActChessBoard::queryReview() {
    if (is_review == true || getWinner() != Winner::none) return;
    QMessageBox msg_box;
    msg_box.setWindowTitle("选择操作");
    msg_box.setText("是否要进行复盘：");
    msg_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Close);
    if (msg_box.exec() == QMessageBox::Ok) {
        review();
    }
}

void IntActChessBoard::review() {
    is_review = true;
    // 防止 change[now_index] 后面还有无关步数。
    while (now_index < change.size() - 1) change.pop_back();
    QDialog dialog;
    QVBoxLayout layout(&dialog);
    QPushButton *forwardButton = new QPushButton("前进", &dialog);
    QPushButton *backwardButton = new QPushButton("后退", &dialog);
    QPushButton *closeButton = new QPushButton("关闭", &dialog);
    layout.addWidget(forwardButton);
    layout.addWidget(backwardButton);
    layout.addWidget(closeButton);
    QObject::connect(forwardButton, &QPushButton::clicked, this, &IntActChessBoard::forwardMove);
    QObject::connect(backwardButton, &QPushButton::clicked, this, &IntActChessBoard::backMove);
    QObject::connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    dialog.exec();
    while (forwardMove());
    is_review = false;
}

NetChessBoard::NetChessBoard(QGraphicsView *board, QLabel *show_text,
    QPushButton *reset, QPushButton *back, Camp cp, QHostAddress ip, quint16 port) :
    IntActChessBoard(board, show_text, reset, back), camp(cp) {
    if (camp == Camp::black) {
        black = new QTcpServer(this);
        black->listen(ip, port);
        connect(black, &QTcpServer::newConnection, this,
                &NetChessBoard::handleConnection);
        QMessageBox::information(nullptr, "提示", "房间创建成功");
    } else if (camp == Camp::red) {
        red = new QTcpSocket();
        red->connectToHost(ip, port);
        connect(red, &QTcpSocket::readyRead, this, &NetChessBoard::handleData);
        QMessageBox::information(nullptr, "提示", "成功加入房间");
    } else {
        QMessageBox::information(nullptr, "警告", "阵营不存在");
    }
}

void NetChessBoard::handlePieceNotice(AbstractChessPiece *piece) {
    if (red == nullptr) {
        QMessageBox::information(nullptr, "警告", "当前玩家人数不足");
    } else {
        if (camp == getCurrCamp()) {
            IntActChessBoard::handlePieceNotice(piece);
        } else {
            if (camp == Camp::black) {
                QMessageBox::information(nullptr, "警告", "当前是红棋回合");
            } else if (camp == Camp::red) {
                QMessageBox::information(nullptr, "警告", "当前是黑棋回合");
            } else {
                QMessageBox::information(nullptr, "警告", "阵营错误");
            }
        }
    }
}

void NetChessBoard::movePiece(QPoint start, QPoint end) {
    sendData("execMove", start, end);
    IntActChessBoard::movePiece(start, end);
}

void NetChessBoard::sendData(QString command) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << command;
    red->write(data);
}

void NetChessBoard::initPieces() {
    sendData("initPieces");
    IntActChessBoard::initPieces();
}

bool NetChessBoard::backMove() {
    sendData("backMove");
    return IntActChessBoard::backMove();
}

bool NetChessBoard::forwardMove() {
    sendData("forwardMove");
    return IntActChessBoard::forwardMove();
}

template <typename... Args>
void NetChessBoard::sendData(QString command, Args... args) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    // 使用展开表达式将每个参数写入流
    out << command;
    (out << ... << args);  // C++17 中的折叠表达式
    red->write(data);
}

void NetChessBoard::handleData() {
    QByteArray data = red->readAll();
    QDataStream in(&data, QIODevice::ReadOnly);
    QString command;
    in >> command;

    // 注意不能使用本类的对应方法，不然会重复发送消息。
    if (command == "initPieces") {
        IntActChessBoard::initPieces();
    } else if (command == "execMove") {
        QPoint start, end;
        in >> start >> end;
        IntActChessBoard::movePiece(start, end);
    } else if (command == "backMove") {
        IntActChessBoard::backMove();
        // NOTE: 赢家会可以优先选择复盘。
    } else if (command == "forwardMove"){
        IntActChessBoard::forwardMove();
    } else {
        QMessageBox::information(nullptr, "警告", "命令错误，没有该命令");
    }
}

// 只能给 black 绑定这个槽函数。
void NetChessBoard::handleConnection() {
    if (red == nullptr) {
        red = black->nextPendingConnection();
        QMessageBox::information(nullptr, "提示", "有玩家进入房间");
        connect(red, &QTcpSocket::readyRead, this, &NetChessBoard::handleData);
        connect(red, &QTcpSocket::disconnected, this, &NetChessBoard::handleDisconnected);
    } else {
        QMessageBox::information(nullptr, "警告", "已经有玩家进入房间");
    }
}

void NetChessBoard::handleDisconnected() {
    delete red;
    red = nullptr;
    QMessageBox::information(nullptr, "警告", "有玩家断开连接");
    IntActChessBoard::initPieces();
}

RobotChessBoard::RobotChessBoard(QGraphicsView *board, QLabel *show_text,
                QPushButton *reset, QPushButton *back) :
                IntActChessBoard(board, show_text, reset, back) {
}

void RobotChessBoard::handlePieceNotice(AbstractChessPiece *piece) {
    if (getCurrCamp() == player_camp) {
        IntActChessBoard::handlePieceNotice(piece);
        robotMove();
    }
}

void RobotChessBoard::robotMove() {
    if (getCurrCamp() != robot_camp || getWinner() != Winner::none) return;
    // 得到机器人可以移动的全部合法路径。
    QVector<std::array<QPoint, 2>> legal_moves = getAllLegalMoves();
    std::array<QPoint, 2> best_move = {QPoint{-1, -1}, QPoint{-1, -1}};
    // 得到当前棋盘分数。
    int best_score = this->evaluateBoard();

    for (const auto& move : legal_moves) {
        int score = best_score;
        // 机器人移动只有两种结果：无变化 / 对方被吃子
        if (getPiece(move[1])->getCamp() == player_camp)
            score += getPiece(move[1])->getScore();
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }
    qDebug() << best_move[0] << " " << best_move[1];
    if (best_move[0] == QPoint{-1, -1}) {
        best_move = legal_moves[rand() % legal_moves.size()];
    }
    movePiece(best_move[0], best_move[1]);
}

QVector<std::array<QPoint, 2>> RobotChessBoard::getAllLegalMoves() {
    QVector<std::array<QPoint, 2>> legal_moves;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            auto piece = getPiece(QPoint(i, j));
            if (piece->getCamp() == robot_camp) {
                for (const auto& move : getPieceLegalPlace(piece)) {
                    legal_moves.push_back({QPoint(i, j), move});
                }
            }
        }
    }
    return legal_moves;
}

QVector<QPoint> RobotChessBoard::getPieceLegalPlace(AbstractChessPiece *piece) {
    QVector<QPoint> place;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (piece->isAbleMove({i, j})) place.append({i, j});
        }
    }
    return place;
}

int RobotChessBoard::evaluateBoard() {
    int total = 0;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            auto piece = getPiece(QPoint(i, j));
            if (piece->getCamp() == robot_camp) {
                total += piece->getScore();
            } else if (piece->getCamp() == player_camp) {
                total -= piece->getScore();
            }
        }
    }
    return total;
}
