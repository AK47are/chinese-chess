#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <QMainWindow>
#include <QGraphicsRectItem>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void trigger();
    void display(vector<vector<bool>>&, vector<vector<QGraphicsRectItem*>>&);
    int check(int, pair<int, int>&, pair<int, int>&);
    void cover(vector<vector<bool>>&, vector<vector<QGraphicsRectItem*>>&, int, int, pair<int, int>, pair<int, int>);
    void block(vector<vector<bool>>&, vector<vector<QGraphicsRectItem*>>&, int, int, pair<int, int>, pair<int, int>);
    void solve(vector<vector<bool>>&, vector<vector<QGraphicsRectItem*>>&, int, pair<int, int>, pair<int, int>);

    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
