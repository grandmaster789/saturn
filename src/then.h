#ifndef SATURN_THEN_H
#define SATURN_THEN_H

#include "connect_result.h"

namespace saturn::then_detail {
    // sprinkle no_unique_address attributes
    template <typename t_Receiver, typename t_Function>
    struct ThenReceiver {
        t_Receiver m_Receiver;
        t_Function m_Function;

        template <typename... t_Values>
        void set_value(t_Values&&... values) noexcept {
            using std::is_void_v;
            using std::invoke_result_t;
            using std::forward;
            using std::current_exception;

            try {
                if constexpr (is_void_v<invoke_result_t<t_Function, t_Values...>>) {
                    m_Function(forward<t_Values>(values)...);
                    m_Receiver.set_value();
                }
                else
                    m_Receiver.set_value(m_Function(forward<t_Values>(values)...));
            }
            catch (...) {
                set_error(current_exception());
            }
        }

        void set_value() noexcept {
            using std::is_void_v;
            using std::invoke_result_t;
            using std::current_exception;

            try {
                if constexpr (is_void_v<invoke_result_t<t_Function>>) {
                    m_Function();
                    m_Receiver.set_value();
                }
                else
                    m_Receiver.set_value(m_Function());
            }
            catch (...) {
                set_error(current_exception());
            }
        }

        void set_error(std::exception_ptr ptr) noexcept {
            m_Receiver.set_error(ptr);
        }

        void set_stopped() noexcept {
            m_Receiver.set_stopped();
        }
    };

    template <
        typename t_Sender,
        typename t_Function,
        typename t_Receiver
    >
    struct ThenOperation {
        connect_result_t<
            t_Sender,
            ThenReceiver<t_Receiver, t_Function>
        > m_Operation;

        void start() noexcept {
            m_Operation.start();
        }
    };

    template <typename t_SenderResult, typename t_Function>
    struct ThenResultType {
        using type = std::invoke_result_t<t_Function, t_SenderResult>;
    };

    template <typename t_Function, typename... Ts>
    struct ThenResultType<std::tuple<Ts...>, t_Function> {
        using type = std::invoke_result_t<t_Function, Ts...>;
    };

    template <typename t_Function>
    struct ThenResultType<void, t_Function> {
        using type = std::invoke_result_t<t_Function>;
    };

    template <
        typename t_Sender,
        typename t_Function
    >
    struct ThenSender {
        using result_t = ThenResultType<typename t_Sender::result_t, t_Function>::type;

        template <typename t_Receiver> using ThenOperation = ThenOperation<t_Sender, t_Function, t_Receiver>;
        template <typename t_Receiver> using ThenReceiver  = ThenReceiver<t_Receiver, t_Function>;

        t_Sender   m_Sender;
        t_Function m_Function;

        template <typename t_Receiver>
        auto connect(t_Receiver&& recv)
            -> ThenOperation<t_Receiver>
        {
            using std::forward;

            return {
                m_Sender.connect(ThenReceiver<t_Receiver> {
                    forward<t_Receiver>(recv),
                    m_Function
                })
            };
        }
    };

    template <typename t_Function>
    struct ThenPartialSender {
        t_Function m_Function;

        template <typename t_Sender>
        auto operator()(t_Sender&& sender) const
            -> ThenSender<t_Sender, t_Function>
        {
            using std::forward;
            using std::move;

            return {
                forward<t_Sender>(sender),
                move(m_Function)
            };
        }
    };
}

namespace saturn {
    template <
        typename t_Sender,
        typename t_Function
    >
    auto then(
        t_Sender&&   sender,
        t_Function&& fn
    )
        -> then_detail::ThenSender<
            std::decay_t<t_Sender>,
            std::decay_t<t_Function>
        >
    {
        return {
            std::forward<t_Sender>(sender),
            std::forward<t_Function>(fn)
        };
    }

    template <typename t_Function>
    auto then(t_Function&& fn)
        -> then_detail::ThenPartialSender<
            std::decay_t<t_Function>
        >
    {
        return {
            std::forward<t_Function>(fn)
        };
    }
}

#endif
