#ifndef SATURN_JUST_H
#define SATURN_JUST_H

#include <utility>
#include <exception>
#include <tuple>
#include <functional>

namespace saturn::just_detail {
    // TODO
    // - restrict all templates to specific concepts

    // CRTP base for pipeline operator support
    template <typename t_CRTP>
    struct JustSenderBase {
        template <typename t_Algorithm>
        friend auto operator | (t_CRTP&& self, t_Algorithm&& algorithm) {
            return algorithm(std::forward<t_CRTP>(self));
        }
    };

    template <
        typename    t_Receiver,
        typename... t_Values
    >
    struct JustOperation {
                              std::tuple<t_Values...> m_Values;
        [[no_unique_address]] t_Receiver              m_Receiver;

        void start() noexcept;
    };

    template <typename t_Receiver>
    struct JustOperation<t_Receiver, void> {
        [[no_unique_address]] t_Receiver m_Receiver;

        void start() noexcept;
    };

    template <typename... t_Values>
    struct JustSender:
        JustSenderBase<JustSender<t_Values...>>
    {
        using result_t = std::tuple<t_Values...>;

        std::tuple<t_Values...> m_Values;

        template <typename t_Receiver>
        auto connect(t_Receiver&& recv)
            -> JustOperation<t_Receiver, t_Values...>;
    };

    template <typename t_SingleValue>
    struct JustSender<t_SingleValue>:
        JustSenderBase<JustSender<t_SingleValue>>
    {
        using result_t = t_SingleValue;

        t_SingleValue m_Value;

        template <typename t_Receiver>
        auto connect(t_Receiver&& recv)
            -> JustOperation<t_Receiver, t_SingleValue>;
    };

    template <>
    struct JustSender<void> {
        using result_t = void;

        template <typename t_Receiver>
        auto connect(t_Receiver&& recv)
            ->JustOperation<t_Receiver, void>;
    };
}

namespace saturn {
    template <typename... Ts>
    auto just(Ts&&... value)
        -> just_detail::JustSender<Ts...>;

    inline auto just()
        -> just_detail::JustSender<void>;
}

#include "just.inl"

#endif
