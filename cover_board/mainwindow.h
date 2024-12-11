#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qgraphicsscene.h>

#include <QGraphicsRectItem>
#include <QMainWindow>

#include "SolveChess.h"
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  void drawNextStep();

 private slots:
  void trigger();

  void on_showStop_clicked();

  void on_showContinue_clicked();

  void on_showBack_clicked();

  void on_showForward_clicked();

 private:
  Ui::MainWindow *ui;
  int draw_count = 0;
  SolveChess *chess;
  QGraphicsScene *scene;
  vector<vector<QGraphicsRectItem *>> cell;
  bool is_stop = false;
};
#endif  // MAINWINDOW_H
