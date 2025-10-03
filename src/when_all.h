#ifndef SATURN_WHEN_ALL_H
#define SATURN_WHEN_ALL_H

#include <type_traits>
#include <tuple>
#include <atomic>
#include <memory>
#include <exception>

#include "connect_result.h"
#include "void_filter.h"

namespace saturn::when_all_detail {
    template <typename... Ts>
    struct WhenAllControl {
        std::atomic<size_t>          m_Remaining = sizeof...(Ts);
        std::atomic<bool>            m_Interrupted = false; // either an error or stopped
        std::exception_ptr           m_Exception;
        void_filtered_tuple_t<Ts...> m_Results;

        bool is_complete() const noexcept {
            return
                m_Remaining.load() == 0 ||
                m_Interrupted.load();
        }
    };

    template <
        typename    t_Control,
        typename    t_Receiver,
        std::size_t t_Index,
        typename... t_Values
    >
    struct WhenAllReceiver {
        std::shared_ptr<t_Control> m_Control;
        t_Receiver&                m_Receiver;

        template <typename T>
        void set_value(T&& value) noexcept {
            try {
                if constexpr (!std::is_void_v<T>) {
                    constexpr size_t idx = IndexVoidFilter<t_Index, t_Values...>::value;

                    std::get<idx>(m_Control->m_Results) = std::forward<T>(value);
                }

                // if this was the last one, notify the outer receiver
                size_t remaining = m_Control->m_Remaining.fetch_sub(1);
                if (remaining == 1 && !m_Control->m_Interrupted.load()) {
                    // if all senders are void, use set_value() without arguments
                    if constexpr (std::tuple_size_v<void_filtered_tuple_t<t_Values...>> == 0) {
                        m_Receiver.set_value();
                    }
                    else {
                        // set the value for those senders that have arguments
                        std::apply([&](auto&&... xs) {
                            m_Receiver.set_value(std::forward<decltype(xs)>(xs)...);
                        }, std::move(m_Control->m_Results));
                    }
                }
            }
            catch (...) {
                set_error(std::current_exception());
            }
        }

        void set_value() noexcept {
            // (void senders)
            // if this was the last one, notify the outer receiver
            size_t remaining = m_Control->m_Remaining.fetch_sub(1);
            if (remaining == 1 && !m_Control->m_Interrupted.load()) {
                // if all senders are void, use set_value() without arguments
                if constexpr (std::tuple_size_v<void_filtered_tuple_t<t_Values...>> == 0) {
                    m_Receiver.set_value();
                }
                else {
                    // set the value for those senders that have arguments
                    std::apply([&](auto&&... xs) {
                        m_Receiver.set_value(std::forward<decltype(xs)>(xs)...);
                    }, std::move(m_Control->m_Results));
                }
            }
        }

        void set_error(std::exception_ptr ptr) noexcept {
            // if there was an earlier interruption, keep it, but otherwise store this exception
            bool expected = false;
            if (m_Control->m_Interrupted.compare_exchange_strong(expected, true)) {
                m_Control->m_Exception = ptr;
                m_Receiver.set_error(ptr);
            }
        }

        void set_stopped() noexcept {
            bool expected = false;
            if (m_Control->m_Interrupted.compare_exchange_strong(expected, true))
                m_Receiver.set_stopped();
        }
    };

    // we'll use specializations trickery to use two parameter packs in the template
    template <
        typename       t_Receiver,
        typename       t_SendersTuple,
        std::size_t... t_Indices
    >
    struct WhenAllOperation{};

    template <
        typename       t_Receiver,
        typename...    t_Senders,
        std::size_t... t_Indices
    >
    struct WhenAllOperation<t_Receiver, std::tuple<t_Senders...>, t_Indices...> {
        using Control = WhenAllControl<
            typename std::remove_cvref_t<t_Senders>::result_t...
        >;

        std::shared_ptr<Control> m_Control;
        std::tuple<
            connect_result_t<
                t_Senders,
                WhenAllReceiver<
                    Control,
                    t_Receiver,
                    t_Indices,
                    typename std::remove_cvref_t<t_Senders>::result_t...
                >
            >...
        > m_Operations;

        WhenAllOperation(
            t_Receiver&                recv,
            std::tuple<t_Senders...>&& senders
        ):
            m_Control(std::make_shared<Control>()),
            m_Operations(
                std::get<t_Indices>(senders).connect(
                    WhenAllReceiver<
                        Control,
                        t_Receiver,
                        t_Indices,
                        typename std::remove_cvref_t<t_Senders>::result_t...
                    > {
                        m_Control,
                        recv
                    }
                )...
            )
        {
        }

        void start() noexcept {
            std::apply([](auto&... op) {
                (op.start(), ...);
            }, m_Operations);
        }
    };

    template <typename... t_Senders>
    struct WhenAllSender {
        using result_t = std::conditional_t<
            std::tuple_size_v<
                void_filtered_tuple_t<
                    typename std::remove_cvref_t<t_Senders>::result_t...
                >
            > == 0,
            // if all senders are void, this sender is void as well
            void,
            // if some senders yield a value, put those in a tuple
            void_filtered_tuple_t<
                typename std::remove_cvref_t<t_Senders>::result_t...
            >
        >;

        std::tuple<t_Senders...> m_Senders;


    private:
        template <typename R, std::size_t... t_Indices>
        auto connect_impl(R&& recv, std::index_sequence<t_Indices...>) {
            return WhenAllOperation<
                std::decay_t<R>,
                std::tuple<t_Senders...>,
                t_Indices...
            > {
                recv,
                std::move(m_Senders)
            };
        }

    public:
        template <typename R>
        auto connect(R&& recv)
        {
            return connect_impl(
                std::forward<R>(recv),
                std::index_sequence_for<t_Senders...>{}
            );
        }
    };
}

namespace saturn {
    template <typename... t_Senders>
    auto when_all(t_Senders&&... senders)
        -> when_all_detail::WhenAllSender<std::remove_cvref_t<t_Senders>...>
    {
        return {
            std::make_tuple(std::forward<t_Senders>(senders)...)
        };
    }
}

#endif
