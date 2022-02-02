# Qt学生管理系统

## 基本源码介绍

* 本项目基于Qt6+cmake，故直接拿到源码是无法跑起来的，需要Qt6的库。上传源码主要是记录源码里面的一些思路。

以下是源码的解析图：（其中的ui布局界面和qss界面美化是最耗时间的部分！）



![QTStudentManager](https://s2.loli.net/2022/02/03/7ntiJcOkEhqQz1M.png)

ui界面布局用到的特殊之处：

1. 重写界面顶部逻辑。
2. 重写一些事件。

数据存储的特殊之处：

* 缓冲区存储

>使用的数据库为Qt自带的sqlite数据库。

## 实现效果

![学生管理系统实录](https://s2.loli.net/2022/02/03/FPWebao4MVwJ37h.gif)