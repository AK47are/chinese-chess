#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "chesspieces.h"
#include <QGraphicsView>
#include <QLabel>
#include <QPushButton>
#include <QTcpServer>

class AbstractChessPiece;

// 可以直接使用该类，也有胜利逻辑和阵营操作逻辑，但是不会有任何输出。
class SimpleChessBoard : public QGraphicsScene {
public:
    static const int COLS = 9;
    static const int ROWS = 10;
    // 格子的长度宽度大小。
    const double WIDTH;
    const double HEIGHT;

    SimpleChessBoard(QGraphicsView *board);

    // 工具函数
    QPointF getPoint(QPoint coord) const;
    AbstractChessPiece *getSelPie() const;
    void setSelPie(AbstractChessPiece *piece);
    AbstractChessPiece *getPiece(QPoint coord) const;
    void setPiece(QPoint coord, AbstractChessPiece *piece);
    Camp getCurrCamp() const;
    enum class Winner {none, black, red};
    Winner getWinner() const;

protected:
    virtual void setView(QGraphicsView *board);
    virtual void drawBoard();
    virtual void initPieces();

    // 逻辑函数
    friend void AbstractChessPiece::notifyChess();
    virtual void handlePieceNotice(AbstractChessPiece *piece);
    void selectPiece(AbstractChessPiece *piece);
    void movePiece(AbstractChessPiece *target);
    void killPiece(AbstractChessPiece *target);

    // 用来添加自定义的棋子。
    void addOtherPiece();

private:

    // 图形坐标与逻辑坐标的映射
    QPointF points[ROWS][COLS];
    AbstractChessPiece *pieces[ROWS][COLS];

    // 记录选择操作选择的 Item 对象。
    AbstractChessPiece *sel_piece = nullptr;

    // 记录当前操作阵营，movePiece(...) 使用。
    Camp curr_camp = Camp::black;

    Winner winner = Winner::none;
};

// 主要补充与窗口中的其他组件的交互，需要更多外部参数。
class IntActChessBoard : public SimpleChessBoard {
public:
    IntActChessBoard(QGraphicsView *board, QLabel *show_text, QPushButton *reset);
    virtual void handlePieceNotice(AbstractChessPiece *piece) override;
    virtual void initPieces() override;
    void showText();

private:
      QLabel *text_ui;
};

// 主要补充有关网络的使用。
class NetChessBoard : public SimpleChessBoard {
public:
    NetChessBoard(QGraphicsView *board, Camp cp, QHostAddress ip, quint16 port);
    virtual void handlePieceNotice(AbstractChessPiece *piece) override;
    QTcpSocket *getSocket();

private slots:
    void onNewConnection();
    void onClientReadyRead();
    void onDisconnected();

private:
    const Camp camp;
    // 默认黑棋为服务器端。
    QTcpServer *black = nullptr;
    QTcpSocket *red = nullptr;
};

#endif // CHESSBOARD_H
