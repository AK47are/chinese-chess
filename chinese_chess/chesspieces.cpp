#include "chesspieces.h"
#include "chessboard.h"

AbstractChessPiece::AbstractChessPiece(SimpleChessBoard &chess, Camp camp_id,
    QPoint coord) : chess_(chess), camp_(camp_id) {

    chess_.addItem(this);
    this->setCoord(coord);
}

void AbstractChessPiece::paint(QPainter *painter,
        const QStyleOptionGraphicsItem *option, QWidget *widget) {

    double size = fmin(chess_.WIDTH, chess_.HEIGHT) - 5;
    QPen pen;
    if (chess_.getSelPie() == this) {
        pen.setColor(Qt::blue);
        pen.setWidth(2);
    } else {
        pen.setColor(Qt::black);
        pen.setWidth(1);
    }
    painter->setBrush(Qt::white);
    painter->setPen(pen);
    // 注意每个参数代表的什么，最好使用官方文档查询。
    painter->drawEllipse(QPointF(0, 0), size / 2, size / 2);
    drawText(painter);
}

void AbstractChessPiece::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    this->notifyChess();
}

void AbstractChessPiece::drawText(QPainter *painter) {
    auto text = getName();
    QString disp_text;
    if (getCamp()== Camp::black) {
        painter->setPen(QPen(Qt::black));
        disp_text = text[0];
    } else if (getCamp() == Camp::red) {
        painter->setPen(QPen(Qt::red));
        disp_text = text[1];
    } else {
        painter->setPen(QPen(Qt::green));
        disp_text = text[2];
    }

    QFont font;
    font.setFamily("FangSong");
    font.setPixelSize(getBoard().WIDTH / (1 + disp_text.size()));
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(QRectF(-100, -100, 200, 200), Qt::AlignCenter,
                      disp_text);
}

std::array<QString, 3> AbstractChessPiece::getName() {
    return {"无", "无", "无"};
}

void AbstractChessPiece::notifyChess() {
    chess_.handlePieceNotice(this);
}

Camp AbstractChessPiece::getCamp() const {
    return camp_;
}

QPoint AbstractChessPiece::getCoord() const {
    return coord_;
}

SimpleChessBoard &AbstractChessPiece::getBoard() const {
    return chess_;
}

void AbstractChessPiece::setCoord(QPoint coord) {
    coord_ = coord;
    // setPos(...) 设置该对象的起点在画布上的坐标，后面 paint(...) 是以起点为基准的相对坐标。
    // 每次设置后会自动调用 paint(...);
    this->setPos(chess_.getPoint(coord_));
    chess_.setPiece(coord_, this);
}

// 碰撞箱
QRectF AbstractChessPiece::boundingRect() const {

    double size = fmin(chess_.WIDTH, chess_.HEIGHT) - 5;
    return {-size / 2, -size / 2, size, size};
}

bool AbstractChessPiece::isAbleMove(QPoint pos) {
    // 同阵营判定
    if (!(chess_.getPiece(pos)->getCamp() != camp_))
        return false;
    return true;
}


bool General::isAbleMove(QPoint pos) {
    if (!AbstractChessPiece::isAbleMove(pos)) return false;
    // 禁止吃子
    if (this->getBoard().getPiece(pos)->getCamp() != Camp::none)
        return false;
    // 区域判定
    if (getCamp() == Camp::red) {
        if (!(pos.x() >= 0 && pos.x() <= 2 && pos.y() >= 3 && pos.y() <= 5))
            return false;
    } else if (getCamp() == Camp::black) {
        if (!((pos.x() >= getBoard().ROWS - 3 && pos.x() <= getBoard().ROWS - 1 &&
               pos.y() >= 3 && pos.y() <= 5)))
            return false;
    }
    // 移动判定
    if (!((pos.x() == getCoord().x() && abs(pos.y() - getCoord().y()) == 1) ||
         (pos.y() == getCoord().y() && abs(pos.x() - getCoord().x()) == 1)))
        return false;
    // 特殊判定：王不见王
    if (rival_general_->getCoord().y() == pos.y()) {
        bool is_meet = false;
        for (int i = fmax(pos.x(), rival_general_->getCoord().x()) - 1;
             fmin(pos.x(), rival_general_->getCoord().x()) < i; --i) {
            if (getBoard().getPiece(QPoint(i, pos.y()))->getCamp() != Camp::none)
                is_meet = true;
        }
        return is_meet;
    }
    return true;
}

bool Guard::isAbleMove(QPoint pos) {
    if (!AbstractChessPiece::isAbleMove(pos)) return false;
    // 区域判定
    if (getCamp() == Camp::red) {
        if (!(pos.x() >= 0 && pos.x() <= 2 && pos.y() >= 3 && pos.y() <= 5))
            return false;
    } else if (getCamp() == Camp::black) {
        if (!((pos.x() >= getBoard().ROWS - 3 && pos.x() <= getBoard().ROWS - 1 &&
               pos.y() >= 3 && pos.y() <= 5)))
            return false;
    }
    // 移动判定
    if (!(abs(pos.x() - getCoord().x()) == 1 && abs(pos.y() - getCoord().y()) == 1))
        return false;
    return true;
}

bool Elephant::isAbleMove(QPoint pos) {
    if (!AbstractChessPiece::isAbleMove(pos)) return false;
    // 区域判定
    if (getCamp() == Camp::red) {
        if (!(pos.x() >= 0 && pos.x() <= getBoard().ROWS / 2 - 1))
            return false;
    } else if (getCamp() == Camp::black) {
        if (!(pos.x() >= getBoard().ROWS / 2 && pos.x() <= getBoard().ROWS - 1))
            return false;
    }
    // 移动判定
    if (!(abs(pos.x() - getCoord().x()) == 2 && abs(pos.y() - getCoord().y()) == 2))
        return false;
    if (!(getBoard().getPiece((pos + getCoord()) / 2)->getCamp() == Camp::none))
        return false;
    return true;
}

bool Horse::isAbleMove(QPoint pos) {
    if (!AbstractChessPiece::isAbleMove(pos)) return false;
    // 移动判定
    if (!((abs(pos.x() - getCoord().x()) == 2 && abs(pos.y() - getCoord().y()) == 1) ||
          (abs(pos.x() - getCoord().x()) == 1 && abs(pos.y() - getCoord().y()) == 2)))
        return false;
    QPoint di = {0, 0}; // 方位
    if (abs(pos.x() - getCoord().x()) == 2) { // 判定为上下移动。
        di = (pos.x() < getCoord().x()) ? QPoint(-1, 0) : QPoint(1, 0);
    } else { // 判定为左右移动。
        di = (pos.y() < getCoord().y()) ? QPoint(0, -1) : QPoint(0, 1);
    }
    if (!(getBoard().getPiece(getCoord() + di)->getCamp() == Camp::none))
        return false;
    return true;
}

bool Chariot::isAbleMove(QPoint pos) {
    if (!AbstractChessPiece::isAbleMove(pos)) return false;
    // 移动判定：在同一横线或竖线
    if (!(pos.x() == getCoord().x() || pos.y() == getCoord().y()))
        return false;
    if (pos.x() == getCoord().x()) {
        for (int i = fmax(pos.y(), getCoord().y()) - 1; fmin(pos.y(), getCoord().y()) < i; --i)
            if (!(getBoard().getPiece(QPoint(pos.x(), i))->getCamp() == Camp::none))
                return false;
    } else {
        for (int i = fmax(pos.x(), getCoord().x()) - 1; fmin(pos.x(), getCoord().x()) < i; --i)
            if (!(getBoard().getPiece(QPoint(i, pos.y()))->getCamp() == Camp::none))
                return false;
    }
    return true;
}

bool Cannon::isAbleMove(QPoint pos) {
    if (!AbstractChessPiece::isAbleMove(pos)) return false;
    if (!(pos.x() == getCoord().x() || pos.y() == getCoord().y()))
        return false;
    // 移动判定：终点位置无棋子
    // 移动判定：终点位置必须有敌方棋子，在同一横线或竖线上，起点和终点间有且仅有一个棋子。
    if (getBoard().getPiece(pos)->getCamp() == Camp::none) {
        if (pos.x() == getCoord().x()) {
            for (int i = fmax(pos.y(), getCoord().y()) - 1; fmin(pos.y(), getCoord().y()) < i; --i)
                if (!(getBoard().getPiece(QPoint(pos.x(), i))->getCamp() == Camp::none))
                    return false;
        } else {
            for (int i = fmax(pos.x(), getCoord().x()) - 1; fmin(pos.x(), getCoord().x()) < i; --i)
                if (!(getBoard().getPiece(QPoint(i, pos.y()))->getCamp() == Camp::none))
                    return false;
        }
    } else {
        // 击杀判定
        int count = 0;
        if (pos.x() == getCoord().x()) {
            for (int i = fmax(pos.y(), getCoord().y()) - 1; fmin(pos.y(), getCoord().y()) < i; --i)
                if (!(getBoard().getPiece(QPoint(pos.x(), i))->getCamp() == Camp::none))
                    ++count;
        } else {
            for (int i = fmax(pos.x(), getCoord().x()) - 1; fmin(pos.x(), getCoord().x()) < i; --i)
                if (!(getBoard().getPiece(QPoint(i, pos.y()))->getCamp() == Camp::none))
                    ++count;
        }
        if (count != 1) return false;
    }
    return true;
}

bool Solider::isAbleMove(QPoint pos) {
    if (!AbstractChessPiece::isAbleMove(pos)) return false;
    // 判定离该棋子距离是否为 1 。
    if (!(abs(pos.x() - getCoord().x()) == 1 ^ abs(pos.y() - getCoord().y()) == 1))
        return false;
    if (pos.x() == getCoord().x()) {// 判定是在 y 轴方向移动。
        if (getCamp() == Camp::red) {
            if (!(getCoord().x() > 4)) // !(过河) = 没过河
                return false;
        } else {
            if (!(getCoord().x() < 5))
                return false;
        }
    } else {
        if (getCamp() == Camp::red) {
            if (!((pos - getCoord()) == QPoint(1, 0)))
                return false;
        } else {
            if (!((pos - getCoord()) == QPoint(-1, 0)))
                return false;
        }
    }
    return true;
}
