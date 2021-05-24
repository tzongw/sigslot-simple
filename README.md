sigslot-simple
==============

simplified sigslot

original version
http://sigslot.sourceforge.net/

Changes:

1. 删除了多线程支持
2. 禁止signal, has_slots的复制构造和赋值
3. slot允许任意返回值，参数个数可少于signal，const成员函数，基类的成员函数
4. signal可以连接signal

Ps:

5. 提供了一个宏SIGNAL(在xsigslot.h中，用法见test.cpp)
6. signal参数不要使用class类型，而应该使用相应的引用或指针(参数在传递过程中会多次复制，使用class类型对效率有影响；另外，代码里有一个trick，所有_connection?的operator()方法在虚表中位置是一样的，只是参数个数不同，为了简化用了同样的方式去调用。msvc下将该方法声明为__cdecl来保证栈平衡，但是多余的参数的析构函数不会被调用，所以使用class类型会有资源泄露。clang下是调用者清栈和析构的，则不会有这个问题。)
