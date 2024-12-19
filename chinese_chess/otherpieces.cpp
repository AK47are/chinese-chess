#include "chessboard.h"
#include "chesspieces.h"


// 棋子必须继承自 AbstractChessPiece 。必须定义 isAbleMove(QFont pos) 。
// 必须为 AbstractChessPiece 提供以下参数。
// AbstractChessPiece(ChessBoard *chess, Camp camp_id, QPoint coord);
// chess 一般为 this;
// Camp 类声明：enum class Camp {none, black, red}; 注意 none 是用来表示空格子的，不能乱用。
// coord 为棋盘上的逻辑坐标，例如第一行第五列为 QFont(0, 4);

// 一些可用方法：
    // ChessBoard *getBoard();
    // Camp getCamp() const;
    // QPoint getCoord() const;
    // void setCoord(QPoint coord); // 会自动设置在棋盘中的一些参数，改变棋子渲染的位置等等。


// 一些可重载的方法：
//  渲染棋子上面的文本。
// drawText(QPainter *painter)

// 指定渲染的文本 返回值为不同阵营，同兵种使用的字, 分别是黑，红, 其他。
// virtual std::array<QString, 3> getName();

// 人机对战使用，用来给人机评估当前局势，值越大人机越倾向击杀。默认返回值为 0 。
// getScore()


// 必须重载
// isAbleMove(QFont pos) 判断是否允许到 pos 。

// 炮灰：可瞬移到敌方任意单位前。无法进行攻击。
class Assassin : public AbstractChessPiece {
public:
    using AbstractChessPiece::AbstractChessPiece;
    virtual std::array<QString, 3> getName() override { return {"炮灰", "炮灰", "无"}; };
    virtual bool isAbleMove(QPoint pos) override;;
    virtual int getScore() const override { return 5; };
};

// 这里可以添加自己的棋子
void SimpleChessBoard::addOtherPiece() {
    // new Assassin(*this, Camp::red, QPoint{1, 4});
    // new Assassin(*this, Camp::black, QPoint{8, 4});
    // 格式类似为：new xxx(*this, camp_id, coord),
}



bool Assassin::isAbleMove(QPoint pos) {
    if (!AbstractChessPiece::isAbleMove(pos))
        return false;
    if (!(getBoard().getPiece(pos)->getCamp() == Camp::none))
        return false;
    QPoint delta[4] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    for (int i = 0; i < 4; ++i) {
        // 越界判定
        if (!((pos + delta[i]).x() < getBoard().ROWS &&
              (pos + delta[i]).x() >= 0 && (pos + delta[i]).y() >= 0 &&
              (pos + delta[i]).y() < getBoard().COLS))
            continue;
        if (getCamp() == Camp::black) {
            if (getBoard().getPiece(pos + delta[i])->getCamp() == Camp::red)
                return true;
        } else {
            if (getBoard().getPiece(pos + delta[i])->getCamp() == Camp::black)
                return true;
        }
    }
    return false;
}
