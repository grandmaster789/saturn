#ifndef SATURN_JUST_INL
#define SATURN_JUST_INL

#include "just.h"

namespace saturn::just_detail {
    template <
        typename    t_Receiver,
        typename... t_Values
    >
    void JustOperation<t_Receiver, t_Values...>::start() noexcept {
        using std::apply;
        using std::forward;
        using std::move;
        using std::current_exception;

        try {
            apply([&](auto&&... val) {
                m_Receiver.set_value(forward<decltype(val)>(val)...);
            }, move(m_Values));
        }
        catch (...) {
            m_Receiver.set_error(current_exception());
        }
    }

    template <typename t_Receiver>
    void JustOperation<t_Receiver, void>::start() noexcept {
        try {
            m_Receiver.set_value();
        }
        catch (...) {
            m_Receiver.set_error(std::current_exception());
        }
    }

    template <typename... t_Values>
    template <typename t_Receiver>
    auto JustSender<t_Values...>::connect(t_Receiver&& recv)
        -> JustOperation<t_Receiver, t_Values...>
    {
        return { m_Values, std::forward<t_Receiver>(recv) };
    }
\
    template <typename t_SingleValue>
    template <typename t_Receiver>
    auto JustSender<t_SingleValue>::connect(t_Receiver&& recv)
        -> JustOperation<t_Receiver, t_SingleValue>
    {
        return { m_Value, std::forward<t_Receiver>(recv) };
    }

    template <typename t_Receiver>
    auto JustSender<void>::connect(t_Receiver&& recv)
        -> JustOperation<t_Receiver, void>
    {
        return { std::forward<t_Receiver>(recv) };
    }
}

namespace saturn {
    template <typename... t_Values>
    auto just(t_Values&&... values)
        -> just_detail::JustSender<t_Values...>
    {
        return  {{}, std::forward<t_Values>(values)... };
    }

    inline auto just()
        -> just_detail::JustSender<void>
    {
        return {};
    }
}

#endif
