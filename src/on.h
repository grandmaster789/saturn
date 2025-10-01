#ifndef SATURN_ON_H
#define SATURN_ON_H

#include <utility>

namespace saturn::on_detail {
    template <
        typename t_Scheduler,
        typename t_Sender,
        typename t_Receiver
    >
    struct OnOperation {
        t_Scheduler m_Scheduler;
        t_Sender    m_Sender;
        t_Receiver  m_Receiver;

        struct OnReceiver {
            OnOperation* m_Operation;

            void set_value() noexcept {
                // when the scheduler completes, start the wrapped sender
                auto wrapped_operation = m_Operation->m_Sender.connect(m_Operation->m_Receiver);
                wrapped_operation.start();
            }

            void set_error(std::exception_ptr exception) noexcept {
                m_Operation->m_Receiver.set_error(exception);
            }

            void set_stopped() noexcept {
                m_Operation->m_Receiver.set_stopped();
            }
        };

        using SchedulerOperationState = decltype(
            std::declval<t_Scheduler>()
            .schedule()
            .connect(std::declval<OnReceiver>())
        );
        SchedulerOperationState m_SchedulerOperationState;

        OnOperation(
            t_Scheduler scheduler,
            t_Sender    sender,
            t_Receiver  receiver
        ):
            m_Scheduler(scheduler),
            m_Sender   (sender),
            m_Receiver (receiver)
        {
            m_SchedulerOperationState = m_Scheduler.schedule().connect(OnReceiver { this });
        }

        void start() noexcept {
            m_SchedulerOperationState.start();
        }
    };

    template <typename t_Scheduler, typename t_Sender>
    struct OnSender {
        using result_t = typename t_Sender::result_t;

        t_Scheduler m_Scheduler;
        t_Sender    m_Sender;

        template <typename t_Receiver>
        auto connect(t_Receiver recv)
            -> OnOperation<t_Scheduler, t_Sender, t_Receiver>
        {
            return {
                m_Scheduler,
                m_Sender,
                recv
            };
        }

        template <typename t_Algorithm>
        friend auto operator | (OnSender&& self, t_Algorithm&& algorithm) {
            return algorithm(std::forward<OnSender>(self));
        }
    };

    template <typename t_Scheduler>
    struct OnPartialSender {
        t_Scheduler m_Scheduler;

        template <typename t_Sender>
        auto operator()(t_Sender sender) const
            -> OnSender<t_Scheduler, t_Sender>
        {
            return  {
                m_Scheduler,
                sender
            };
        }
    };
}

namespace saturn {
    template <
        typename t_Scheduler,
        typename t_Sender
    >
    auto on(t_Scheduler sched, t_Sender sender)
        -> on_detail::OnSender<t_Scheduler, t_Sender>
    {
        return {
            sched,
            sender
        };
    }

    template <typename t_Scheduler>
    auto on(t_Scheduler&& sched)
        -> on_detail::OnPartialSender<t_Scheduler>
    {
        return {
            sched
        };
    }
}

#endif
