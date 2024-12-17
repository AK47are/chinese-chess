#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QPaintEvent>
#include <QGraphicsView>
class AbstractChessPiece;
enum class Piece;

// 负责渲染棋盘以及棋子的移动
class ChessBoard : public QGraphicsScene {
public:
    static const int COLS = 9;
    static const int ROWS = 10;
    // 格子的长度宽度大小。
    const double WIDTH;
    const double HEIGHT;
    friend class AbstractChessPiece;

    ChessBoard(QGraphicsView *board);

    void judgeSelectOrMove(AbstractChessPiece *piece);
    void selectPiece(AbstractChessPiece *piece);
    void movePiece(AbstractChessPiece *target);
    void killPiece(AbstractChessPiece *target);

    QPointF getPoint(QPoint coord);
    AbstractChessPiece *&getPiece(QPoint coord);

    // 用来添加自定义的棋子。
    void addOtherPiece();
private:
    virtual void initPieces();
    virtual void drawBoard(QGraphicsView *board);

    QPointF points[ROWS][COLS];
    // NOTE: 不初始化在 initPieces() 添加 NoPiece 对象时判断可能出错。
    AbstractChessPiece *pieces[ROWS][COLS] = {nullptr};
    AbstractChessPiece *sel_piece = nullptr;
};

#endif // CHESSBOARD_H
