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
    // void setCoord(QPoint coord); // 会自动设置在棋盘中的一些参数。


// 一些可重载的方法：
//  渲染棋子上面的文本。
// drawText(QPainter *painter)
// 指定渲染的文本 返回值为不同阵营，同兵种使用的字, 分别是黑，红, 其他。
// virtual std::array<QString, 3> getName();

// 必须重载
// isAbleMove(QFont pos) 判断是否允许到 pos 。

class Super : public AbstractChessPiece {
public:
    using AbstractChessPiece::AbstractChessPiece;
    virtual std::array<QString, 3> getName() override { return {"机甲", "马", "无"}; };
    virtual bool isAbleMove(QPoint pos) override { return true; };
};

// 这里可以添加自己的棋子
void ChessBoard::addOtherPiece() {
    // new Super(this, Camp::black, QPoint{5, 5});
    // 格式类似为：new xxx(this, camp_id, coord),
}
