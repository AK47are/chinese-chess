#include "solvechess.h"

#include <array>
#include <utility>
#include <vector>
using namespace std;

SolveChess::SolveChess(int size, pair<int, int> spec_blk) {
  change.push_back({spec_blk, spec_blk, spec_blk});
  solve(size, {0, 0}, spec_blk);
}

int SolveChess::check(int N, pair<int, int>& coord, pair<int, int>& spec_blk) {
  enum { LEFT_TOP, RIGHT_TOP, LEFT_BOTTOM, RIGHT_BOTTOM };
  if (spec_blk.first - coord.first < N / 2) {
    if (spec_blk.second - coord.second < N / 2)
      return LEFT_TOP;
    else
      return RIGHT_TOP;
  } else {
    if (spec_blk.second - coord.second < N / 2)
      return LEFT_BOTTOM;
    else
      return RIGHT_BOTTOM;
  }
}

// 覆盖，记录下来。
void SolveChess::cover(int size, int pos, pair<int, int> coord) {
  int block_dx[4] = {size / 2 - 1, size / 2 - 1, size / 2, size / 2};
  int block_dy[4] = {size / 2 - 1, size / 2, size / 2 - 1, size / 2};
  int top = 0;
  array<pair<int, int>, 3> arr;
  for (int i = 0; i < 4; ++i) {
    if (i == pos) continue;
    arr[top++] = {coord.first + block_dx[i], coord.second + block_dy[i]};
  }
  change.push_back(arr);
}

// 分块
void SolveChess::block(int size, int pos, pair<int, int> coord,
                       pair<int, int> spec_blk) {
  int coord_dx[4] = {0, 0, size / 2, size / 2};
  int coord_dy[4] = {0, size / 2, 0, size / 2};
  int block_dx[4] = {size / 2 - 1, size / 2 - 1, size / 2, size / 2};
  int block_dy[4] = {size / 2 - 1, size / 2, size / 2 - 1, size / 2};
  solve(size / 2, {coord.first + coord_dx[pos], coord.second + coord_dy[pos]},
        {spec_blk.first, spec_blk.second});
  for (int i = 0; i < 4; ++i) {
    if (i == pos) continue;
    solve(size / 2, {coord.first + coord_dx[i], coord.second + coord_dy[i]},
          {coord.first + block_dx[i], coord.second + block_dy[i]});
  }
}

void SolveChess::solve(int size, pair<int, int> coord, pair<int, int> blk) {
  if (size == 1) return;
  int pos = check(size, coord, blk);
  cover(size, pos, coord);
  block(size, pos, coord, blk);
}
