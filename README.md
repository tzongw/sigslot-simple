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
