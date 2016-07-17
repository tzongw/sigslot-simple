#ifndef XSIGSLOT_H__
#define XSIGSLOT_H__

#include "sigslot.h"

namespace xsigslot {

template <class F>
class xsignal;

template <>
class xsignal<void ()>
    : public sigslot::signal0
{};

template <class a1_t>
class xsignal<void (a1_t)>
    : public sigslot::signal1<a1_t>
{};

template <class a1_t, class a2_t>
class xsignal<void (a1_t, a2_t)>
    : public sigslot::signal2<a1_t, a2_t>
{};

template <class a1_t, class a2_t, class a3_t>
class xsignal<void (a1_t, a2_t, a3_t)>
    : public sigslot::signal3<a1_t, a2_t, a3_t>
{};

template <class a1_t, class a2_t, class a3_t, class a4_t>
class xsignal<void (a1_t, a2_t, a3_t, a4_t)>
    : public sigslot::signal4<a1_t, a2_t, a3_t, a4_t>
{};

template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t>
class xsignal<void (a1_t, a2_t, a3_t, a4_t, a5_t)>
    : public sigslot::signal5<a1_t, a2_t, a3_t, a4_t, a5_t>
{};

template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t, class a6_t>
class xsignal<void (a1_t, a2_t, a3_t, a4_t, a5_t, a6_t)>
    : public sigslot::signal6<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>
{};

template <class a1_t, class a2_t, class a3_t, class a4_t, class a5_t, class a6_t, class a7_t>
class xsignal<void (a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t)>
    : public sigslot::signal7<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>
{};

template <class a1_t, class a2_t, class a3_t, class a4_t,
    class a5_t, class a6_t, class a7_t, class a8_t>
class xsignal<void (a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t)>
    : public sigslot::signal8<a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>
{};

} // namespace xsigslot

// macro to define signal
#define SIGNAL(Name, ...) \
public: \
    xsigslot::xsignal<void (__VA_ARGS__)>::base *sig##Name() const { return &Name; } \
protected: \
    mutable xsigslot::xsignal<void (__VA_ARGS__)> Name;

#endif // XSIGSLOT_H__
