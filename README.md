## 说明
存放数据结构与算法课程的课程设计：可视化棋盘覆盖；可视化中国象棋。

使用 Qt C++来制作图形化交互，在 Qt Creator 中编写。

使用方法：在 Qt Creator 中点击对应项目的 CmakeLists.txt 即可打开项目。

## 特点

#### 中国象棋
  - 代码包含大量注释说明。
  - 代码进行过多次优化，使其尽可能清晰。严格限定了各函数和变量的访问权限。
  - 支持单机对战、网络对战、人机对战。
  - 实现悔棋、复盘功能等，参见 commit 提交记录。
  - 支持添加自定义棋子，参见 otheripeces.cpp 。
  - 使用类继承实现棋盘的不同功能模块。模块功能间是隔离的。
  - 棋盘类使用简单，只要符合构造函数，即可直接使用。
  - 支持自定义棋盘显示大小和位置、组件布局。
  - 棋子移动判定和绘画由棋子自身定义，而不是棋盘类负责。
  - 鼠标事件由棋子监控，传递给棋盘处理。

## 课题

### 棋盘覆盖
<div style="text-align: center;">
    <img src="docs/image/课程设计_棋盘覆盖_问题描述.png">
    <img src="docs/image/课程设计_棋盘覆盖_问题分析.png">
    <img src="docs/image/课程设计_棋盘覆盖_问题要求.png">
</div>

### 中国象棋
<div style="text-align: center;">
  <img src="docs/image/课程设计_中国象棋_问题描述1.png">
  <img src="docs/image/课程设计_中国象棋_问题描述2.png">
  <img src="docs/image/课程设计_中国象棋_问题要求.png">
</div>
