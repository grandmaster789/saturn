#ifndef SATURN_JUST_H
#define SATURN_JUST_H

#include <utility>
#include <stdexcept>

namespace saturn::just_detail {
    // - we could replace the single-value propagation with std::tuple
    // - we could make the operation internal to the sender (but it's already in a detail namespace)
    // - we could introduce the [[no_unique_address]] tag for the receiver storage
    // - we could restrict all templates to specific concepts

    template <typename R, typename T>
    struct JustOperation {
        T m_Value;
        R m_Receiver;

        void start() noexcept {
            try {
                m_Receiver.set_value(std::move(m_Value));
            }
            catch (...) {
                m_Receiver.set_error(std::current_exception());
            }
        }
    };

    template <typename R>
    struct JustOperation<R, void> {
        R m_Receiver;

        void start() noexcept {
            try {
                m_Receiver.set_value();
            }
            catch (...) {
                m_Receiver.set_error(std::current_exception());
            }
        }
    };

    template <typename T>
    struct JustSender {
        using result_t = T;

        T m_Values;

        template <typename R>
        auto connect(R&& recv)
            -> JustOperation<R, T>
        {
            return JustOperation<R, T>(
                m_Values,
                std::forward<R>(recv)
            );
        }

        template <typename t_Algorithm>
        friend auto operator | (JustSender&& self, t_Algorithm&& algorithm) {
            return algorithm(std::forward<JustSender>(self));
        }
    };

    template <>
    struct JustSender<void> {
        using result_t = void;

        template <typename R>
        auto connect(R&& recv)
            ->JustOperation<R, void>
        {
            return JustOperation<R, void>(
                std::forward<R>(recv)
            );
        }
    };
}

namespace saturn {
    template <typename T>
    auto just(T&& value)
        -> just_detail::JustSender<T>
    {
        return just_detail::JustSender<T> {
            std::forward<T>(value)
        };
    }

    inline auto just()
        -> just_detail::JustSender<void>
    {
        return just_detail::JustSender<void> {};
    }
}

#endif
