#ifndef SIGSLOT_H__
#define SIGSLOT_H__

#include <set>
#include <list>
#include <cassert>

#define SIGSLOT_DISABLE_COPY(Class) \
private: \
    Class(const Class&); \
    Class& operator=(const Class&);

#define SIGSLOT_DEFAULT_CONSTRUCTOR(Class) \
public: \
    Class() {} \
SIGSLOT_DISABLE_COPY(Class)

namespace sigslot {

class _signal_base;

class has_slots
{
    SIGSLOT_DEFAULT_CONSTRUCTOR(has_slots)
    friend class _signal_base;
public:
    ~has_slots();
private:
    void _signal_connect(_signal_base *sender)
    {
        m_senders.insert(sender);
    }
    void _signal_disconnect(_signal_base *sender)
    {
        m_senders.erase(sender);
    }
	typedef std::set<_signal_base *> sender_set;
	sender_set m_senders;
};

class _connection_base
{
    SIGSLOT_DISABLE_COPY(_connection_base)
public:
    _connection_base(int argc) : m_argc(argc) {}
    virtual ~_connection_base() {}
    virtual has_slots *dest() const = 0;
    int argc() const { return m_argc; }
private:
    int m_argc;
};

class _signal_base
{
    SIGSLOT_DEFAULT_CONSTRUCTOR(_signal_base)
public:
    ~_signal_base()
    {
        for (connections_list::const_iterator i = m_connected_slots.begin();
            i != m_connected_slots.end(); ++i)
        {
            (*i)->dest()->_signal_disconnect(this);
            delete *i;
        }
    }
    void disconnect(const has_slots *pclass)
    {
        for (connections_list::iterator i = m_connected_slots.begin();
            i != m_connected_slots.end();)
        {
            if ((*i)->dest() == pclass)
            {
                (*i)->dest()->_signal_disconnect(this);
                delete *i;
                i = m_connected_slots.erase(i);
            }
            else ++i;
        }
    }
protected:
    void _connect_to_has_slots(has_slots *pclass)
    {
        pclass->_signal_connect(this);
    }
    typedef std::list<_connection_base *> connections_list;
    connections_list m_connected_slots;
};


inline has_slots::~has_slots()
{   // 这里不能用iterator来遍历，因为disconnect会调用_signal_disconnect
    // m_senders会被修改，导致iterator失效
    while (!m_senders.empty())
        (*m_senders.begin())->disconnect(this);
}

//////////////////////////////////////////////////
class _connection_base0 : public _connection_base
{
public:
	_connection_base0() : _connection_base(0) {}
	virtual void operator()() const = 0;
};

template <class dest_t, class ret_t>
class _connection0 : public _connection_base0
{
public:
    _connection0(dest_t *pobject, ret_t (dest_t::*pmemfun)())
    {
        m_pobject = pobject;
        m_pmemfun = pmemfun;
    }
	virtual void operator()() const
	{
	    (m_pobject->*m_pmemfun)();
	}
    virtual has_slots *dest() const
    {
        return m_pobject;
    }
private:
    dest_t *m_pobject;
    ret_t (dest_t::*m_pmemfun)();
};

class signal0;

class _signal_base0 : public _signal_base
{
public:
    template <class Dest_t, class dest_t, class ret_t>
    void connect(Dest_t *pclass, ret_t (dest_t::*pmemfun)())
    {
        _connect_to_has_slots(pclass);
        m_connected_slots.push_back(new _connection0<Dest_t, ret_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass, ret_t (dest_t::*pmemfun)() const)
    {
        // Warning: 这里把const强转掉，不知有没有副作用。。。
        connect((Dest_t *)pclass, (ret_t (dest_t::*)())pmemfun);
    }

    void connect(signal0 *pclass);
};

class signal0 : public _signal_base0, public has_slots
{
public:
    void operator()() const
    {   // 或者这里用dynamic_cast？
        for (connections_list::const_iterator i = m_connected_slots.begin();
            i != m_connected_slots.end(); ++i)
        {
            switch ((*i)->argc())
            {
            case 0:
            	(*(_connection_base0 *)(*i))();
            	break;
            default:
            	assert(false);
            }
        }
    }
    typedef _signal_base0 base;
};

inline void _signal_base0::connect(signal0 *pclass)
{
    connect(pclass, &signal0::operator());
}

//////////////////////////////////////////////////
template <class a1_t>
class _connection_base1 : public _connection_base
{
public:
	_connection_base1() : _connection_base(1) {}
	virtual void operator()(a1_t a1) const = 0;
};

template <class dest_t, class ret_t, class a1_t>
class _connection1 : public _connection_base1<a1_t>
{
public:
    _connection1(dest_t *pobject, ret_t (dest_t::*pmemfun)(a1_t))
    {
        m_pobject = pobject;
        m_pmemfun = pmemfun;
    }
	virtual void operator()(a1_t a1) const
	{
	    (m_pobject->*m_pmemfun)(a1);
	}
    virtual has_slots *dest() const
    {
        return m_pobject;
    }
private:
    dest_t *m_pobject;
    ret_t (dest_t::*m_pmemfun)(a1_t);
};

template <class a1_t>
class signal1;

template <class a1_t>
class _signal_base1 : public _signal_base0
{
public:
    template <class Dest_t, class dest_t, class ret_t>
    void connect(Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t))
    {
        _connect_to_has_slots(pclass);
        m_connected_slots.push_back(new _connection1<Dest_t, ret_t, a1_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t) const)
    {
        connect((Dest_t *)pclass, (ret_t (dest_t::*)(a1_t))pmemfun);
    }

    void connect(signal1<a1_t> *pclass)
    {
        connect(pclass, &signal1<a1_t>::operator());
    }
    _signal_base0::connect;
};

template <class a1_t>
class signal1 : public _signal_base1<a1_t>, public has_slots
{
public:
    void operator()(a1_t a1) const
    {
        for (typename base::connections_list::const_iterator i = this->m_connected_slots.begin();
            i != this->m_connected_slots.end(); ++i)
        {
            switch ((*i)->argc())
            {
            case 0:
            	(*(_connection_base0 *)(*i))();
            	break;
            case 1:
            	(*(_connection_base1<a1_t> *)(*i))(a1);
            	break;
            default:
            	assert(false);
            }
        }
    }
    typedef _signal_base1<a1_t> base;
};

//////////////////////////////////////////////////
template <class a1_t, class a2_t>
class _connection_base2 : public _connection_base
{
public:
	_connection_base2() : _connection_base(2) {}
	virtual void operator()(a1_t a1, a2_t a2) const = 0;
};

template <class dest_t, class ret_t, class a1_t, class a2_t>
class _connection2 : public _connection_base2<a1_t, a2_t>
{
public:
    _connection2(dest_t *pobject, ret_t (dest_t::*pmemfun)(a1_t, a2_t))
    {
        m_pobject = pobject;
        m_pmemfun = pmemfun;
    }
	virtual void operator()(a1_t a1, a2_t a2) const
	{
	    (m_pobject->*m_pmemfun)(a1, a2);
	}
    virtual has_slots *dest() const
    {
        return m_pobject;
    }
private:
    dest_t *m_pobject;
    ret_t (dest_t::*m_pmemfun)(a1_t, a2_t);
};

template <class a1_t, class a2_t>
class signal2;

template <class a1_t, class a2_t>
class _signal_base2 : public _signal_base1<a1_t>
{
public:
    template <class Dest_t, class dest_t, class ret_t>
    void connect(Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t, a2_t))
    {
        _connect_to_has_slots(pclass);
        this->m_connected_slots.push_back(
            new _connection2<Dest_t, ret_t, a1_t, a2_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t, a2_t) const)
    {
        connect((Dest_t *)pclass, (ret_t (dest_t::*)(a1_t, a2_t))pmemfun);
    }

    void connect(signal2<a1_t, a2_t> *pclass)
    {
        connect(pclass, &signal2<a1_t, a2_t>::operator());
    }
    _signal_base1<a1_t>::connect;
};

template <class a1_t, class a2_t>
class signal2 : public _signal_base2<a1_t, a2_t>, public has_slots
{
public:
    void operator()(a1_t a1, a2_t a2) const
    {
        for (typename base::connections_list::const_iterator i = this->m_connected_slots.begin();
            i != this->m_connected_slots.end(); ++i)
        {
            switch ((*i)->argc())
            {
            case 0:
            	(*(_connection_base0 *)(*i))();
            	break;
            case 1:
            	(*(_connection_base1<a1_t> *)(*i))(a1);
            	break;
            case 2:
                (*(_connection_base2<a1_t, a2_t> *)(*i))(a1, a2);
                break;
            default:
            	assert(false);
            }
        }
    }
    typedef _signal_base2<a1_t, a2_t> base;
};

//////////////////////////////////////////////////
template <class a1_t, class a2_t, class a3_t>
class _connection_base3 : public _connection_base
{
public:
	_connection_base3() : _connection_base(3) {}
	virtual void operator()(a1_t a1, a2_t a2, a3_t a3) const = 0;
};

template <class dest_t, class ret_t, class a1_t, class a2_t, class a3_t>
class _connection3 : public _connection_base3<a1_t, a2_t, a3_t>
{
public:
    _connection3(dest_t *pobject, ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t))
    {
        m_pobject = pobject;
        m_pmemfun = pmemfun;
    }
	virtual void operator()(a1_t a1, a2_t a2, a3_t a3) const
	{
	    (m_pobject->*m_pmemfun)(a1, a2, a3);
	}
    virtual has_slots *dest() const
    {
        return m_pobject;
    }
private:
    dest_t *m_pobject;
    ret_t (dest_t::*m_pmemfun)(a1_t, a2_t, a3_t);
};

template <class a1_t, class a2_t, class a3_t>
class signal3;

template <class a1_t, class a2_t, class a3_t>
class _signal_base3 : public _signal_base2<a1_t, a2_t>
{
public:
    template <class Dest_t, class dest_t, class ret_t>
    void connect(Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t))
    {
        _connect_to_has_slots(pclass);
        this->m_connected_slots.push_back(
            new _connection3<Dest_t, ret_t, a1_t, a2_t, a3_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass,
        ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t) const)
    {
        connect((Dest_t *)pclass, (ret_t (dest_t::*)(a1_t, a2_t, a3_t))pmemfun);
    }

    void connect(signal3<a1_t, a2_t, a3_t> *pclass)
    {
        connect(pclass, &signal3<a1_t, a2_t, a3_t>::operator());
    }
    _signal_base2<a1_t, a2_t>::connect;
};

template <class a1_t, class a2_t, class a3_t>
class signal3 : public _signal_base3<a1_t, a2_t, a3_t>, public has_slots
{
public:
    void operator()(a1_t a1, a2_t a2, a3_t a3) const
    {
        for (typename base::connections_list::const_iterator i = this->m_connected_slots.begin();
            i != this->m_connected_slots.end(); ++i)
        {
            switch ((*i)->argc())
            {
            case 0:
            	(*(_connection_base0 *)(*i))();
            	break;
            case 1:
            	(*(_connection_base1<a1_t> *)(*i))(a1);
            	break;
            case 2:
                (*(_connection_base2<a1_t, a2_t> *)(*i))(a1, a2);
                break;
            case 3:
                (*(_connection_base3<a1_t, a2_t, a3_t> *)(*i))(a1, a2, a3);
                break;
            default:
            	assert(false);
            }
        }
    }
    typedef _signal_base3<a1_t, a2_t, a3_t> base;
};

//////////////////////////////////////////////////
template <class a1_t, class a2_t, class a3_t, class a4_t>
class _connection_base4 : public _connection_base
{
public:
	_connection_base4() : _connection_base(4) {}
	virtual void operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4) const = 0;
};

template <class dest_t, class ret_t, class a1_t, class a2_t, class a3_t, class a4_t>
class _connection4 : public _connection_base4<a1_t, a2_t, a3_t, a4_t>
{
public:
    _connection4(dest_t *pobject,
        ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t, a4_t))
    {
        m_pobject = pobject;
        m_pmemfun = pmemfun;
    }
	virtual void operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4) const
	{
	    (m_pobject->*m_pmemfun)(a1, a2, a3, a4);
	}
    virtual has_slots *dest() const
    {
        return m_pobject;
    }
private:
    dest_t *m_pobject;
    ret_t (dest_t::*m_pmemfun)(a1_t, a2_t, a3_t, a4_t);
};

template <class a1_t, class a2_t, class a3_t, class a4_t>
class signal4;

template <class a1_t, class a2_t, class a3_t, class a4_t>
class _signal_base4 : public _signal_base3<a1_t, a2_t, a3_t>
{
public:
    template <class Dest_t, class dest_t, class ret_t>
    void connect(Dest_t *pclass,
        ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t, a4_t))
    {
        _connect_to_has_slots(pclass);
        this->m_connected_slots.push_back(
            new _connection4<Dest_t,
            ret_t, a1_t, a2_t, a3_t, a4_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass,
        ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t, a4_t) const)
    {
        connect((Dest_t *)pclass,
            (ret_t (dest_t::*)(a1_t, a2_t, a3_t, a4_t))pmemfun);
    }

    void connect(signal4<a1_t, a2_t, a3_t, a4_t> *pclass)
    {
        connect(pclass, &signal4<a1_t, a2_t, a3_t, a4_t>::operator());
    }
    _signal_base3<a1_t, a2_t, a3_t>::connect;
};

template <class a1_t, class a2_t, class a3_t, class a4_t>
class signal4 : public _signal_base4<a1_t, a2_t, a3_t, a4_t>, public has_slots
{
public:
    void operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4) const
    {
        for (typename base::connections_list::const_iterator i = this->m_connected_slots.begin();
            i != this->m_connected_slots.end(); ++i)
        {
            switch ((*i)->argc())
            {
            case 0:
            	(*(_connection_base0 *)(*i))();
            	break;
            case 1:
            	(*(_connection_base1<a1_t> *)(*i))(a1);
            	break;
            case 2:
                (*(_connection_base2<a1_t, a2_t> *)(*i))(a1, a2);
                break;
            case 3:
                (*(_connection_base3<a1_t, a2_t, a3_t> *)(*i))(a1, a2, a3);
                break;
            case 4:
                (*(_connection_base4<a1_t, a2_t, a3_t, a4_t> *)(*i))(a1, a2, a3, a4);
                break;
            default:
            	assert(false);
            }
        }
    }
    typedef _signal_base4<a1_t, a2_t, a3_t, a4_t> base;
};

//////////////////////////////////////////////////
template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t>
class _connection_base5 : public _connection_base
{
public:
	_connection_base5() : _connection_base(5) {}
	virtual void operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5) const = 0;
};

template <class dest_t, class ret_t, class a1_t, class a2_t,
    class a3_t, class a4_t, class a5_t>
class _connection5 : public _connection_base5<a1_t, a2_t, a3_t, a4_t, a5_t>
{
public:
    _connection5(dest_t *pobject,
        ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t, a4_t, a5_t))
    {
        m_pobject = pobject;
        m_pmemfun = pmemfun;
    }
	virtual void operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5) const
	{
	    (m_pobject->*m_pmemfun)(a1, a2, a3, a4, a5);
	}
    virtual has_slots *dest() const
    {
        return m_pobject;
    }
private:
    dest_t *m_pobject;
    ret_t (dest_t::*m_pmemfun)(a1_t, a2_t, a3_t, a4_t, a5_t);
};

template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t>
class signal5;

template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t>
class _signal_base5 : public _signal_base4<a1_t, a2_t, a3_t, a4_t>
{
public:
    template <class Dest_t, class dest_t, class ret_t>
    void connect(Dest_t *pclass,
        ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t, a4_t, a5_t))
    {
        _connect_to_has_slots(pclass);
        this->m_connected_slots.push_back(
            new _connection5<Dest_t,
            ret_t, a1_t, a2_t, a3_t, a4_t, a5_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass,
        ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t, a4_t, a5_t) const)
    {
        connect((Dest_t *)pclass,
            (ret_t (dest_t::*)(a1_t, a2_t, a3_t, a4_t, a5_t))pmemfun);
    }

    void connect(signal5<a1_t, a2_t, a3_t, a4_t, a5_t> *pclass)
    {
        connect(pclass, &signal5<a1_t, a2_t, a3_t, a4_t, a5_t>::operator());
    }
    _signal_base4<a1_t, a2_t, a3_t, a4_t>::connect;
};

template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t>
class signal5 : public _signal_base5<a1_t, a2_t, a3_t, a4_t, a5_t>, public has_slots
{
public:
    void operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5) const
    {
        for (typename base::connections_list::const_iterator i = this->m_connected_slots.begin();
            i != this->m_connected_slots.end(); ++i)
        {
            switch ((*i)->argc())
            {
            case 0:
            	(*(_connection_base0 *)(*i))();
            	break;
            case 1:
            	(*(_connection_base1<a1_t> *)(*i))(a1);
            	break;
            case 2:
                (*(_connection_base2<a1_t, a2_t> *)(*i))(a1, a2);
                break;
            case 3:
                (*(_connection_base3<a1_t, a2_t, a3_t> *)(*i))(a1, a2, a3);
                break;
            case 4:
                (*(_connection_base4<a1_t, a2_t, a3_t, a4_t> *)(*i))(a1, a2, a3, a4);
                break;
            case 5:
                (*(_connection_base5<a1_t, a2_t, a3_t,
                    a4_t, a5_t> *)(*i))(a1, a2, a3, a4, a5);
                break;
            default:
            	assert(false);
            }
        }
    }
    typedef _signal_base5<a1_t, a2_t, a3_t, a4_t, a5_t> base;
};

//////////////////////////////////////////////////
template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t, class a6_t>
class _connection_base6 : public _connection_base
{
public:
	_connection_base6() : _connection_base(6) {}
	virtual void operator()(a1_t a1, a2_t a2, a3_t a3,
	    a4_t a4, a5_t a5, a6_t a6) const = 0;
};

template <class dest_t, class ret_t, class a1_t, class a2_t,
    class a3_t, class a4_t, class a5_t, class a6_t>
class _connection6 : public _connection_base6<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>
{
public:
    _connection6(dest_t *pobject,
        ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t, a4_t, a5_t, a6_t))
    {
        m_pobject = pobject;
        m_pmemfun = pmemfun;
    }
	virtual void operator()(a1_t a1, a2_t a2, a3_t a3,
	    a4_t a4, a5_t a5, a6_t a6) const
	{
	    (m_pobject->*m_pmemfun)(a1, a2, a3, a4, a5, a6);
	}
    virtual has_slots *dest() const
    {
        return m_pobject;
    }
private:
    dest_t *m_pobject;
    ret_t (dest_t::*m_pmemfun)(a1_t, a2_t, a3_t, a4_t, a5_t, a6_t);
};

template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t, class a6_t>
class signal6;

template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t, class a6_t>
class _signal_base6 : public _signal_base5<a1_t, a2_t, a3_t, a4_t, a5_t>
{
public:
    template <class Dest_t, class dest_t, class ret_t>
    void connect(Dest_t *pclass,
        ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t, a4_t, a5_t, a6_t))
    {
        _connect_to_has_slots(pclass);
        this->m_connected_slots.push_back(new _connection6<Dest_t,
            ret_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass,
        ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t, a4_t, a5_t, a6_t) const)
    {
        connect((Dest_t *)pclass,
            (ret_t (dest_t::*)(a1_t, a2_t, a3_t, a4_t, a5_t, a6_t))pmemfun);
    }

    void connect(signal6<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t> *pclass)
    {
        connect(pclass, &signal6<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::operator());
    }
    _signal_base5<a1_t, a2_t, a3_t, a4_t, a5_t>::connect;
};

template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t, class a6_t>
class signal6 : public _signal_base6<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>,
                public has_slots
{
public:
    void operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6) const
    {
        for (typename base::connections_list::const_iterator i = this->m_connected_slots.begin();
            i != this->m_connected_slots.end(); ++i)
        {
            switch ((*i)->argc())
            {
            case 0:
            	(*(_connection_base0 *)(*i))();
            	break;
            case 1:
            	(*(_connection_base1<a1_t> *)(*i))(a1);
            	break;
            case 2:
                (*(_connection_base2<a1_t, a2_t> *)(*i))(a1, a2);
                break;
            case 3:
                (*(_connection_base3<a1_t, a2_t, a3_t> *)(*i))(a1, a2, a3);
                break;
            case 4:
                (*(_connection_base4<a1_t, a2_t, a3_t, a4_t> *)(*i))(a1, a2, a3, a4);
                break;
            case 5:
                (*(_connection_base5<a1_t, a2_t, a3_t,
                    a4_t, a5_t> *)(*i))(a1, a2, a3, a4, a5);
                break;
            case 6:
                (*(_connection_base6<a1_t, a2_t, a3_t,
                    a4_t, a5_t, a6_t> *)(*i))(a1, a2, a3, a4, a5, a6);
                break;
            default:
            	assert(false);
            }
        }
    }
    typedef _signal_base6<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t> base;
};

//////////////////////////////////////////////////
template <class a1_t, class a2_t, class a3_t, class a4_t,
    class a5_t, class a6_t, class a7_t>
class _connection_base7 : public _connection_base
{
public:
	_connection_base7() : _connection_base(7) {}
	virtual void operator()(a1_t a1, a2_t a2, a3_t a3,
	    a4_t a4, a5_t a5, a6_t a6, a7_t a7) const = 0;
};

template <class dest_t, class ret_t, class a1_t, class a2_t,
    class a3_t, class a4_t, class a5_t, class a6_t, class a7_t>
class _connection7 : public _connection_base7<a1_t, a2_t,
    a3_t, a4_t, a5_t, a6_t, a7_t>
{
public:
    _connection7(dest_t *pobject, ret_t (dest_t::*pmemfun)(a1_t,
        a2_t, a3_t, a4_t, a5_t, a6_t, a7_t))
    {
        m_pobject = pobject;
        m_pmemfun = pmemfun;
    }
	virtual void operator()(a1_t a1, a2_t a2, a3_t a3,
	    a4_t a4, a5_t a5, a6_t a6, a7_t a7) const
	{
	    (m_pobject->*m_pmemfun)(a1, a2, a3, a4, a5, a6, a7);
	}
    virtual has_slots *dest() const
    {
        return m_pobject;
    }
private:
    dest_t *m_pobject;
    ret_t (dest_t::*m_pmemfun)(a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t);
};

template <class a1_t, class a2_t, class a3_t, class a4_t,
    class a5_t, class a6_t, class a7_t>
class signal7;

template <class a1_t, class a2_t, class a3_t, class a4_t,
    class a5_t, class a6_t, class a7_t>
class _signal_base7 : public _signal_base6<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>
{
public:
    template <class Dest_t, class dest_t, class ret_t>
    void connect(Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t,
        a2_t, a3_t, a4_t, a5_t, a6_t, a7_t))
    {
        _connect_to_has_slots(pclass);
        this->m_connected_slots.push_back(new _connection7<Dest_t, ret_t, a1_t, a2_t,
            a3_t, a4_t, a5_t, a6_t, a7_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t,
        a2_t, a3_t, a4_t, a5_t, a6_t, a7_t) const)
    {
        connect((Dest_t *)pclass, (ret_t (dest_t::*)(a1_t, a2_t, a3_t,
            a4_t, a5_t, a6_t, a7_t))pmemfun);
    }

    void connect(signal7<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> *pclass)
    {
        connect(pclass, &signal7<a1_t, a2_t, a3_t, a4_t,
            a5_t, a6_t, a7_t>::operator());
    }
    _signal_base6<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::connect;
};

template <class a1_t, class a2_t, class a3_t, class a4_t,
    class a5_t, class a6_t, class a7_t>
class signal7 : public _signal_base7<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>,
                public has_slots
{
public:
    void operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4,
        a5_t a5, a6_t a6, a7_t a7) const
    {
        for (typename base::connections_list::const_iterator i = this->m_connected_slots.begin();
            i != this->m_connected_slots.end(); ++i)
        {
            switch ((*i)->argc())
            {
            case 0:
            	(*(_connection_base0 *)(*i))();
            	break;
            case 1:
            	(*(_connection_base1<a1_t> *)(*i))(a1);
            	break;
            case 2:
                (*(_connection_base2<a1_t, a2_t> *)(*i))(a1, a2);
                break;
            case 3:
                (*(_connection_base3<a1_t, a2_t, a3_t> *)(*i))(a1, a2, a3);
                break;
            case 4:
                (*(_connection_base4<a1_t, a2_t, a3_t, a4_t> *)(*i))(a1, a2, a3, a4);
                break;
            case 5:
                (*(_connection_base5<a1_t, a2_t, a3_t,
                    a4_t, a5_t> *)(*i))(a1, a2, a3, a4, a5);
                break;
            case 6:
                (*(_connection_base6<a1_t, a2_t, a3_t,
                    a4_t, a5_t, a6_t> *)(*i))(a1, a2, a3, a4, a5, a6);
                break;
            case 7:
                (*(_connection_base7<a1_t, a2_t, a3_t,
                    a4_t, a5_t, a6_t, a7_t> *)(*i))(a1, a2, a3, a4, a5, a6, a7);
                break;
            default:
            	assert(false);
            }
        }
    }
    typedef _signal_base7<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> base;
};

//////////////////////////////////////////////////
template <class a1_t, class a2_t, class a3_t, class a4_t,
    class a5_t, class a6_t, class a7_t, class a8_t>
class _connection_base8 : public _connection_base
{
public:
	_connection_base8() : _connection_base(8) {}
	virtual void operator()(a1_t a1, a2_t a2, a3_t a3,
	    a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8) const = 0;
};

template <class dest_t, class ret_t, class a1_t, class a2_t,
    class a3_t, class a4_t, class a5_t, class a6_t, class a7_t, class a8_t>
class _connection8 : public _connection_base8<a1_t, a2_t,
    a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>
{
public:
    _connection8(dest_t *pobject, ret_t (dest_t::*pmemfun)(a1_t,
        a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t))
    {
        m_pobject = pobject;
        m_pmemfun = pmemfun;
    }
	virtual void operator()(a1_t a1, a2_t a2, a3_t a3,
	    a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8) const
	{
	    (m_pobject->*m_pmemfun)(a1, a2, a3, a4, a5, a6, a7, a8);
	}
    virtual has_slots *dest() const
    {
        return m_pobject;
    }
private:
    dest_t *m_pobject;
    ret_t (dest_t::*m_pmemfun)(a1_t, a2_t, a3_t,
        a4_t, a5_t, a6_t, a7_t, a8_t);
};

template <class a1_t, class a2_t, class a3_t, class a4_t,
    class a5_t, class a6_t, class a7_t, class a8_t>
class signal8;

template <class a1_t, class a2_t, class a3_t, class a4_t,
    class a5_t, class a6_t, class a7_t, class a8_t>
class _signal_base8 :
    public _signal_base7<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>
{
public:
    template <class Dest_t, class dest_t, class ret_t>
    void connect(Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t,
        a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t))
    {
        _connect_to_has_slots(pclass);
        this->m_connected_slots.push_back(new _connection8<Dest_t, ret_t, a1_t, a2_t,
            a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t,
        a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t) const)
    {
        connect((Dest_t *)pclass, (ret_t (dest_t::*)(a1_t, a2_t, a3_t,
            a4_t, a5_t, a6_t, a7_t, a8_t))pmemfun);
    }

    void connect(signal8<a1_t, a2_t, a3_t, a4_t,
        a5_t, a6_t, a7_t, a8_t> *pclass)
    {
        connect(pclass, &signal8<a1_t, a2_t, a3_t, a4_t,
            a5_t, a6_t, a7_t, a8_t>::operator());
    }
    _signal_base7<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>::connect;
};

template <class a1_t, class a2_t, class a3_t, class a4_t,
    class a5_t, class a6_t, class a7_t, class a8_t>
class signal8 : public _signal_base8<a1_t, a2_t, a3_t, a4_t,
    a5_t, a6_t, a7_t, a8_t>, public has_slots
{
public:
    void operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4,
        a5_t a5, a6_t a6, a7_t a7, a8_t a8) const
    {
        for (typename base::connections_list::const_iterator i = this->m_connected_slots.begin();
            i != this->m_connected_slots.end(); ++i)
        {
            switch ((*i)->argc())
            {
            case 0:
            	(*(_connection_base0 *)(*i))();
            	break;
            case 1:
            	(*(_connection_base1<a1_t> *)(*i))(a1);
            	break;
            case 2:
                (*(_connection_base2<a1_t, a2_t> *)(*i))(a1, a2);
                break;
            case 3:
                (*(_connection_base3<a1_t, a2_t, a3_t> *)(*i))(a1, a2, a3);
                break;
            case 4:
                (*(_connection_base4<a1_t, a2_t, a3_t, a4_t> *)(*i))(a1, a2, a3, a4);
                break;
            case 5:
                (*(_connection_base5<a1_t, a2_t, a3_t,
                    a4_t, a5_t> *)(*i))(a1, a2, a3, a4, a5);
                break;
            case 6:
                (*(_connection_base6<a1_t, a2_t, a3_t,
                    a4_t, a5_t, a6_t> *)(*i))(a1, a2, a3, a4, a5, a6);
                break;
            case 7:
                (*(_connection_base7<a1_t, a2_t, a3_t,
                    a4_t, a5_t, a6_t, a7_t> *)(*i))(a1, a2, a3, a4, a5, a6, a7);
                break;
            case 8:
                (*(_connection_base8<a1_t, a2_t, a3_t,
                    a4_t, a5_t, a6_t, a7_t, a8_t> *)(*i))(a1, a2, a3, a4, a5, a6, a7, a8);
                break;
            default:
            	assert(false);
            }
        }
    }
    typedef _signal_base8<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> base;
};

} // namespace sigslot

#endif // SIGSLOT_H__
