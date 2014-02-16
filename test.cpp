#include <iostream>
#include "xsigslot.h"

using namespace std;

class A
{
	SIGNAL(Changed, int newState, int oldState)
public:
	A(int state = 0) : m_state(state) {}
	void set(int state)
	{
		if (state != m_state)
		{
			int old = m_state;
			m_state = state;
			Changed(m_state, old);
		}
	}
private:
	int	m_state;
};

class B
{
public:
	void trace(int state) const { cout << "current: " << state << endl; }
	void what() const { cout << "what!!!" << endl; }
};

class C : public B, public sigslot::has_slots
{
	SIGNAL(MemberChanged)
public:
	C()
	{
		a.sigChanged()->connect(this, &C::trace);  // base, const, less param
		a.sigChanged()->connect(this, &C::delta);  // return int; 允许复制构造这个连接要怎么处理？
		a.sigChanged()->connect(&MemberChanged);   // to signal
	}
	int delta(int left, int right) const
	{
		int del = left - right;
		cout << "delta: " << del << endl;
		return del;
	}
	void setA(int state) { a.set(state); }
private:
	A	a;
};

int main()
{
	C c;
	c.sigMemberChanged()->connect(&c, &C::what);
	c.setA(1);
	c.setA(10);
	return 0;
}
