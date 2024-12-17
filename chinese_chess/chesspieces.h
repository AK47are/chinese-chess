#ifndef CHESSPIECES_H
#define CHESSPIECES_H

#include <QFont>
#include <QPainter>
#include <QGraphicsItem>

class ChessBoard;
enum class Camp {none, black, red};

class AbstractChessPiece : public QGraphicsItem {

public:
    explicit AbstractChessPiece(ChessBoard *chess, Camp camp_id, QPoint coord);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    QRectF boundingRect() const override final;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override final;

    virtual void drawText(QPainter *painter);
    virtual std::array<QString, 3> getName();

    virtual bool isAbleMove(QPoint pos) = 0;

    ChessBoard *getBoard();
    Camp getCamp() const;
    QPoint getCoord() const;
    void setCoord(QPoint coord);

private:
    ChessBoard *chess_ = nullptr;
    const Camp camp_ = Camp::none;
    QPoint coord_ ; // 逻辑坐标
};

class NoPiece : public AbstractChessPiece {

public:
    NoPiece(ChessBoard *chess, QPoint coord) : AbstractChessPiece(chess,
                             Camp::none, coord) {};
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override {
        // 用来 debug
        // painter->setBrush(Qt::black);
        // painter->drawRect(boundingRect());
    };
    virtual bool isAbleMove(QPoint pos) override { return false; };
};

class General : public AbstractChessPiece {
public:
    using AbstractChessPiece::AbstractChessPiece;
    void setRival(AbstractChessPiece *rival_general) {rival_general_ = rival_general;};
    virtual std::array<QString, 3> getName() override { return {"将", "帅", "无"}; };
    virtual bool isAbleMove(QPoint pos) override;

private:
    AbstractChessPiece *rival_general_;
};

class Guard : public AbstractChessPiece {
public:
    using AbstractChessPiece::AbstractChessPiece;
    virtual std::array<QString, 3> getName() override { return {"士", "仕", "无"}; };
    virtual bool isAbleMove(QPoint pos) override;
};

class Elephant : public AbstractChessPiece {
public:
    using AbstractChessPiece::AbstractChessPiece;
    virtual std::array<QString, 3> getName() override { return {"象", "相", "无"}; };
    virtual bool isAbleMove(QPoint pos) override;
};

class Horse : public AbstractChessPiece {
public:
    using AbstractChessPiece::AbstractChessPiece;
    virtual std::array<QString, 3> getName() override { return {"马", "马", "无"}; };
    virtual bool isAbleMove(QPoint pos) override;
};

class Chariot : public AbstractChessPiece {
public:
    using AbstractChessPiece::AbstractChessPiece;
    virtual std::array<QString, 3> getName() override { return {"车", "车", "无"}; };
    virtual bool isAbleMove(QPoint pos) override;
};

class Cannon : public AbstractChessPiece {
public:
    using AbstractChessPiece::AbstractChessPiece;
    virtual std::array<QString, 3> getName() override { return {"炮", "炮", "无"}; };
    virtual bool isAbleMove(QPoint pos) override;
};

class Solider : public AbstractChessPiece {
public:
    using AbstractChessPiece::AbstractChessPiece;
    virtual std::array<QString, 3> getName() override { return {"卒", "兵", "无"}; };
    virtual bool isAbleMove(QPoint pos) override;
};

#endif // CHESSPIECES_H
