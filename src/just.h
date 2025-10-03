#ifndef SATURN_JUST_H
#define SATURN_JUST_H

#include <utility>
#include <exception>
#include <tuple>
#include <functional>

namespace saturn::just_detail {
    // - introduce the [[no_unique_address]] tag for the receiver storage
    // - restrict all templates to specific concepts
    // - NOTE check if the std::apply works in c++17 (lambda template)

    template <typename R, typename... Ts>
    struct JustOperation {
        std::tuple<Ts...> m_Values;
        R                 m_Receiver;

        void start() noexcept {
            using std::apply;
            using std::forward;
            using std::move;
            using std::current_exception;

            try {
				apply([&]<typename... T0>(T0&&... val) {
					m_Receiver.set_value(forward<T0>(val)...);
				}, move(m_Values));
            }
            catch (...) {
                m_Receiver.set_error(current_exception());
            }
        }
    };

    template <typename R>
    struct JustOperation<R, void> {
        R m_Receiver;

        void start() noexcept {
            using std::current_exception;

            try {
                m_Receiver.set_value();
            }
            catch (...) {
                m_Receiver.set_error(current_exception());
            }
        }
    };

    template <typename... Ts>
    struct JustSender {
        using result_t = std::tuple<Ts...>;

        std::tuple<Ts...> m_Values;

        template <typename R>
        auto connect(R&& recv)
            -> JustOperation<R, Ts...>
        {
            using std::forward;

            return {
                m_Values,
                forward<R>(recv)
            };
        }

        template <typename t_Algorithm>
        friend auto operator | (JustSender&& self, t_Algorithm&& algorithm) {
            using std::forward;

            return algorithm(
                forward<JustSender>(self)
            );
        }
    };

    template <typename T>
    struct JustSender<T> {
        using result_t = T;

        T m_Value;

        template <typename R>
        auto connect(R&& recv)
            -> JustOperation<R, T>
        {
            using std::forward;

            return {
                m_Value,
                forward<R>(recv)
            };
        }

        template <typename t_Algorithm>
        friend auto operator | (JustSender&& self, t_Algorithm&& algorithm) {
            using std::forward;

            return algorithm(
                forward<JustSender>(self)
            );
        }
    };

    template <>
    struct JustSender<void> {
        using result_t = void;

        template <typename R>
        auto connect(R&& recv)
            ->JustOperation<R, void>
        {
            using std::forward;

            return { forward<R>(recv) };
        }
    };
}

namespace saturn {
    template <typename... Ts>
    auto just(Ts&&... value)
        -> just_detail::JustSender<Ts...>
    {
        return  { std::forward<Ts>(value)... };
    }

    inline auto just()
        -> just_detail::JustSender<void>
    {
        return {};
    }
}

#endif
