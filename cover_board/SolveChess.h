#ifndef SOLVECHESS_H
#define SOLVECHESS_H
#include <utility>
#include <vector>
using namespace std;
class SolveChess {
 public:
  vector<array<pair<int, int>, 3>> change;

 public:
  SolveChess(int size, pair<int, int> spec_blk);
  int check(int, pair<int, int>&, pair<int, int>&);
  void cover(int, int, pair<int, int>);
  void block(int, int, pair<int, int>, pair<int, int>);
  void solve(int, pair<int, int>, pair<int, int>);
};

#endif  // SOLVECHESS_H
