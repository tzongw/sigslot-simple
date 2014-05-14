sigslot-simple
==============

simplified sigslot

original version
http://sigslot.sourceforge.net/

Changes:
1. 删除了多线程支持
2. 禁止signal, has_slots的复制构造和赋值
3. slot允许任意返回值，参数个数少于signal，const成员函数，基类的成员函数
4. signal可以连接signal

Ps:
5. 提供了一个或许有用的宏SIGNAL(在xsigslot.h中，用法见test.cpp)
6. 发现一个隐患，在slot中调用disconnect会崩溃，用connections_list的副本可以避免，但会损失效率
