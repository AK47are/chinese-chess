#include "widget.h"
#include "chessboard.h"
#include "./ui_widget.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget) {
    ui->setupUi(this);
    initChessBoard();
}

Widget::~Widget() {
    delete ui;
}

// 在图形化界面让用户选择。当然你直接初始化也行，不过也要指定的参数。
void Widget::initChessBoard() {
    this->setWindowTitle("中国象棋");

    QMessageBox msg_box(this);
    msg_box.setWindowTitle("选择操作");
    msg_box.setText("请选择您要执行的操作");
    QPushButton* offline = msg_box.addButton("单机对战", QMessageBox::ActionRole);
    QPushButton* online = msg_box.addButton("在线对战", QMessageBox::ActionRole);
    msg_box.addButton(QMessageBox::Close);
    msg_box.exec();

    if (msg_box.clickedButton() == offline) {
        // 需要指定组件名字。
        new IntActChessBoard(ui->chessboard, ui->chess_text, ui->reset, ui->back);
    } else if (msg_box.clickedButton() == online) {
        QMessageBox msg_box(this);
        msg_box.setWindowTitle("选择操作");
        msg_box.setText("请选择您要执行的操作");
        QPushButton* create = msg_box.addButton("创建房间", QMessageBox::ActionRole);
        QPushButton* join = msg_box.addButton("加入房间", QMessageBox::ActionRole);
        msg_box.addButton(QMessageBox::Close);
        msg_box.exec();
        if (msg_box.clickedButton() == create) {
            // 注意 使用 vpn 会找不到，可能是代理服务器的问题。
            new NetActChessBoard(ui->chessboard, ui->chess_text, ui->reset, ui->back,
                                 Camp::black, QHostAddress::Any, 11111);
        } else if (msg_box.clickedButton() == join){
            bool is_ip, is_port, is_convert;
            QHostAddress ip = QHostAddress(QInputDialog::getText(this, tr("输入框"),
                 tr("请输入对应 ip"), QLineEdit::Normal, "", &is_ip));
            quint16 port = (QInputDialog::getText(this, tr("输入框"),
                tr("请输入对应 port"), QLineEdit::Normal, "", &is_port)).toUInt(&is_convert);
            if (is_ip && !ip.isNull() && is_port && is_convert && port <= 65535) {
                new NetActChessBoard(ui->chessboard, ui->chess_text, ui->reset,
                                  ui->back, Camp::red, ip, port);
            } else {
                QMessageBox::warning(nullptr, "警告", "连接失败，非法输入！");
                exit(0);
            }
        } else {
            exit(0);
        }
    } else {
        exit(0);
    }
}
