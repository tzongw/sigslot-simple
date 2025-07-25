#ifndef SIGSLOT_H__
#define SIGSLOT_H__

#include <set>
#include <vector>

#define SIGSLOT_DISABLE_COPY(Class) \
private: \
    Class(const Class&); \
    Class& operator=(const Class&);

#define SIGSLOT_DEFAULT_CONSTRUCTOR(Class) \
public: \
    Class() {} \
SIGSLOT_DISABLE_COPY(Class)

#ifdef _MSC_VER
    #define SIGSLOT_INVOKE __cdecl
#else
    #define SIGSLOT_INVOKE
#endif

namespace sigslot {

class _signal_base;

class has_slots
{
    SIGSLOT_DEFAULT_CONSTRUCTOR(has_slots)
    friend class _signal_base;
public:
    void free_slots();
    ~has_slots() { free_slots(); }
private:
    void _signal_connect(_signal_base *sender)
    {
        m_senders.insert(sender);
    }
    void _signal_disconnect(_signal_base *sender)
    {
        m_senders.erase(sender);
    }
    typedef std::set<_signal_base *> senders;
    senders m_senders;
};

class _connection_base
{
    SIGSLOT_DEFAULT_CONSTRUCTOR(_connection_base)
public:
    virtual ~_connection_base() {}
    virtual has_slots *dest() const = 0;
};

class _signal_base
{
    SIGSLOT_DEFAULT_CONSTRUCTOR(_signal_base)
public:
    ~_signal_base()
    {
        for (size_t i = 0; i < m_connected_slots.size(); ++i)
        {
            _connection_base* const conn = m_connected_slots[i];
            if (conn == NULL) continue;
            conn->dest()->_signal_disconnect(this);
            delete conn;
        }
    }
    void disconnect(const has_slots *pclass)
    {
        for (size_t i = 0; i < m_connected_slots.size(); ++i)
        {
            _connection_base* const conn = m_connected_slots[i];
            if (conn && conn->dest() == pclass)
            {
                conn->dest()->_signal_disconnect(this);
                delete conn;
                m_connected_slots[i] = NULL;
            }
        }
    }
protected:
    void _insert_connection(_connection_base *connection)
    {
        for (size_t i = 0; i < m_connected_slots.size(); ++i)
        {
            _connection_base* const conn = m_connected_slots[i];
            if (conn == NULL)
            {
                m_connected_slots[i] = connection;
                return;
            }
        }
        m_connected_slots.push_back(connection);
    }
    void _connect_to_has_slots(has_slots *pclass)
    {
        pclass->_signal_connect(this);
    }
    typedef std::vector<_connection_base *> connections;
    connections m_connected_slots;
};


inline void has_slots::free_slots()
{
    while (!m_senders.empty())
        (*m_senders.begin())->disconnect(this);
}

//////////////////////////////////////////////////
class _connection_base0 : public _connection_base
{
public:
    virtual void SIGSLOT_INVOKE operator()() const = 0;
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
    virtual void SIGSLOT_INVOKE operator()() const
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
        _insert_connection(new _connection0<Dest_t, ret_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass, ret_t (dest_t::*pmemfun)() const)
    {
        connect((Dest_t *)pclass, (ret_t (dest_t::*)())pmemfun);
    }

    void connect(const signal0 *pclass);
};

class signal0 : public _signal_base0, public has_slots
{
public:
    void operator()() const
    {
        for (size_t i = 0; i < m_connected_slots.size(); ++i)
        {
            _connection_base* const conn = m_connected_slots[i];
            if (conn == NULL) continue;
            (*(_connection_base0 *)conn)();
        }
    }
    typedef _signal_base0 base;
};

inline void _signal_base0::connect(const signal0 *pclass)
{
    connect(pclass, &signal0::operator());
}

//////////////////////////////////////////////////
template <class a1_t>
class _connection_base1 : public _connection_base
{
public:
    virtual void SIGSLOT_INVOKE operator()(a1_t a1) const = 0;
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
    virtual void SIGSLOT_INVOKE operator()(a1_t a1) const
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
        _insert_connection(new _connection1<Dest_t, ret_t, a1_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t) const)
    {
        connect((Dest_t *)pclass, (ret_t (dest_t::*)(a1_t))pmemfun);
    }

    void connect(const signal1<a1_t> *pclass)
    {
        connect(pclass, &signal1<a1_t>::operator());
    }
    using _signal_base0::connect;
};

template <class a1_t>
class signal1 : public _signal_base1<a1_t>, public has_slots
{
public:
    void operator()(a1_t a1) const
    {
        for (size_t i = 0; i < this->m_connected_slots.size(); ++i)
        {
            _connection_base* const conn = this->m_connected_slots[i];
            if (conn == NULL) continue;
            (*(_connection_base1<a1_t> *)conn)(a1);
        }
    }
    typedef _signal_base1<a1_t> base;
};

//////////////////////////////////////////////////
template <class a1_t, class a2_t>
class _connection_base2 : public _connection_base
{
public:
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2) const = 0;
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
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2) const
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
        this->_connect_to_has_slots(pclass);
        this->_insert_connection(
            new _connection2<Dest_t, ret_t, a1_t, a2_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t, a2_t) const)
    {
        connect((Dest_t *)pclass, (ret_t (dest_t::*)(a1_t, a2_t))pmemfun);
    }

    void connect(const signal2<a1_t, a2_t> *pclass)
    {
        connect(pclass, &signal2<a1_t, a2_t>::operator());
    }
    using _signal_base1<a1_t>::connect;
};

template <class a1_t, class a2_t>
class signal2 : public _signal_base2<a1_t, a2_t>, public has_slots
{
public:
    void operator()(a1_t a1, a2_t a2) const
    {
        for (size_t i = 0; i < this->m_connected_slots.size(); ++i)
        {
            _connection_base* const conn = this->m_connected_slots[i];
            if (conn == NULL) continue;
            (*(_connection_base2<a1_t, a2_t> *)conn)(a1, a2);
        }
    }
    typedef _signal_base2<a1_t, a2_t> base;
};

//////////////////////////////////////////////////
template <class a1_t, class a2_t, class a3_t>
class _connection_base3 : public _connection_base
{
public:
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2, a3_t a3) const = 0;
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
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2, a3_t a3) const
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
        this->_connect_to_has_slots(pclass);
        this->_insert_connection(
            new _connection3<Dest_t, ret_t, a1_t, a2_t, a3_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass,
        ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t) const)
    {
        connect((Dest_t *)pclass, (ret_t (dest_t::*)(a1_t, a2_t, a3_t))pmemfun);
    }

    void connect(const signal3<a1_t, a2_t, a3_t> *pclass)
    {
        connect(pclass, &signal3<a1_t, a2_t, a3_t>::operator());
    }
    using _signal_base2<a1_t, a2_t>::connect;
};

template <class a1_t, class a2_t, class a3_t>
class signal3 : public _signal_base3<a1_t, a2_t, a3_t>, public has_slots
{
public:
    void operator()(a1_t a1, a2_t a2, a3_t a3) const
    {
        for (size_t i = 0; i < this->m_connected_slots.size(); ++i)
        {
            _connection_base* const conn = this->m_connected_slots[i];
            if (conn == NULL) continue;
            (*(_connection_base3<a1_t, a2_t, a3_t> *)conn)(a1, a2, a3);
        }
    }
    typedef _signal_base3<a1_t, a2_t, a3_t> base;
};

//////////////////////////////////////////////////
template <class a1_t, class a2_t, class a3_t, class a4_t>
class _connection_base4 : public _connection_base
{
public:
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4) const = 0;
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
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4) const
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
        this->_connect_to_has_slots(pclass);
        this->_insert_connection(
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

    void connect(const signal4<a1_t, a2_t, a3_t, a4_t> *pclass)
    {
        connect(pclass, &signal4<a1_t, a2_t, a3_t, a4_t>::operator());
    }
    using _signal_base3<a1_t, a2_t, a3_t>::connect;
};

template <class a1_t, class a2_t, class a3_t, class a4_t>
class signal4 : public _signal_base4<a1_t, a2_t, a3_t, a4_t>, public has_slots
{
public:
    void operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4) const
    {
        for (size_t i = 0; i < this->m_connected_slots.size(); ++i)
        {
            _connection_base* const conn = this->m_connected_slots[i];
            if (conn == NULL) continue;
            (*(_connection_base4<a1_t, a2_t, a3_t, a4_t> *)conn)(a1, a2, a3, a4);
        }
    }
    typedef _signal_base4<a1_t, a2_t, a3_t, a4_t> base;
};

//////////////////////////////////////////////////
template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t>
class _connection_base5 : public _connection_base
{
public:
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5) const = 0;
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
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5) const
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
        this->_connect_to_has_slots(pclass);
        this->_insert_connection(
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

    void connect(const signal5<a1_t, a2_t, a3_t, a4_t, a5_t> *pclass)
    {
        connect(pclass, &signal5<a1_t, a2_t, a3_t, a4_t, a5_t>::operator());
    }
    using _signal_base4<a1_t, a2_t, a3_t, a4_t>::connect;
};

template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t>
class signal5 : public _signal_base5<a1_t, a2_t, a3_t, a4_t, a5_t>, public has_slots
{
public:
    void operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5) const
    {
        for (size_t i = 0; i < this->m_connected_slots.size(); ++i)
        {
            _connection_base* const conn = this->m_connected_slots[i];
            if (conn == NULL) continue;
            (*(_connection_base5<a1_t, a2_t, a3_t,
                a4_t, a5_t> *)conn)(a1, a2, a3, a4, a5);
        }
    }
    typedef _signal_base5<a1_t, a2_t, a3_t, a4_t, a5_t> base;
};

//////////////////////////////////////////////////
template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t, class a6_t>
class _connection_base6 : public _connection_base
{
public:
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2, a3_t a3,
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
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2, a3_t a3,
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
        this->_connect_to_has_slots(pclass);
        this->_insert_connection(new _connection6<Dest_t,
            ret_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass,
        ret_t (dest_t::*pmemfun)(a1_t, a2_t, a3_t, a4_t, a5_t, a6_t) const)
    {
        connect((Dest_t *)pclass,
            (ret_t (dest_t::*)(a1_t, a2_t, a3_t, a4_t, a5_t, a6_t))pmemfun);
    }

    void connect(const signal6<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t> *pclass)
    {
        connect(pclass, &signal6<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::operator());
    }
    using _signal_base5<a1_t, a2_t, a3_t, a4_t, a5_t>::connect;
};

template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t, class a6_t>
class signal6 : public _signal_base6<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>,
                public has_slots
{
public:
    void operator()(a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6) const
    {
        for (size_t i = 0; i < this->m_connected_slots.size(); ++i)
        {
            _connection_base* const conn = this->m_connected_slots[i];
            if (conn == NULL) continue;
            (*(_connection_base6<a1_t, a2_t, a3_t,
                a4_t, a5_t, a6_t> *)conn)(a1, a2, a3, a4, a5, a6);
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
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2, a3_t a3,
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
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2, a3_t a3,
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
        this->_connect_to_has_slots(pclass);
        this->_insert_connection(new _connection7<Dest_t, ret_t, a1_t, a2_t,
            a3_t, a4_t, a5_t, a6_t, a7_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t,
        a2_t, a3_t, a4_t, a5_t, a6_t, a7_t) const)
    {
        connect((Dest_t *)pclass, (ret_t (dest_t::*)(a1_t, a2_t, a3_t,
            a4_t, a5_t, a6_t, a7_t))pmemfun);
    }

    void connect(const signal7<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> *pclass)
    {
        connect(pclass, &signal7<a1_t, a2_t, a3_t, a4_t,
            a5_t, a6_t, a7_t>::operator());
    }
    using _signal_base6<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::connect;
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
        for (size_t i = 0; i < this->m_connected_slots.size(); ++i)
        {
            _connection_base* const conn = this->m_connected_slots[i];
            if (conn == NULL) continue;
            (*(_connection_base7<a1_t, a2_t, a3_t,
                a4_t, a5_t, a6_t, a7_t> *)conn)(a1, a2, a3, a4, a5, a6, a7);
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
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2, a3_t a3,
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
    virtual void SIGSLOT_INVOKE operator()(a1_t a1, a2_t a2, a3_t a3,
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
        this->_connect_to_has_slots(pclass);
        this->_insert_connection(new _connection8<Dest_t, ret_t, a1_t, a2_t,
            a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>(pclass, pmemfun));
    }

    template <class Dest_t, class dest_t, class ret_t>
    void connect(const Dest_t *pclass, ret_t (dest_t::*pmemfun)(a1_t,
        a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t) const)
    {
        connect((Dest_t *)pclass, (ret_t (dest_t::*)(a1_t, a2_t, a3_t,
            a4_t, a5_t, a6_t, a7_t, a8_t))pmemfun);
    }

    void connect(const signal8<a1_t, a2_t, a3_t, a4_t,
        a5_t, a6_t, a7_t, a8_t> *pclass)
    {
        connect(pclass, &signal8<a1_t, a2_t, a3_t, a4_t,
            a5_t, a6_t, a7_t, a8_t>::operator());
    }
    using _signal_base7<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>::connect;
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
        for (size_t i = 0; i < this->m_connected_slots.size(); ++i)
        {
            _connection_base* const conn = this->m_connected_slots[i];
            if (conn == NULL) continue;
            (*(_connection_base8<a1_t, a2_t, a3_t,
                a4_t, a5_t, a6_t, a7_t, a8_t> *)conn)(a1, a2, a3, a4, a5, a6, a7, a8);
        }
    }
    typedef _signal_base8<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> base;
};

} // namespace sigslot

#endif // SIGSLOT_H__
