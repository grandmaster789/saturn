#ifndef SATURN_INLINE_SCHEDULER_H
#define SATURN_INLINE_SCHEDULER_H

#include <utility>

namespace saturn::inline_scheduler_detail {
    template <typename R>
    struct InlineSchedulerOperation {
        R m_Receiver;

        void start() noexcept {
            m_Receiver.set_value();
        }
    };

    struct InlineSchedulerSender {
        using result_t = void;

        template <typename R>
        auto connect(R&& recv) {
            return InlineSchedulerOperation<std::decay_t<R>>{
                std::forward<R>(recv)
            };
        }

        template <typename t_Algorithm>
        friend auto operator | (InlineSchedulerSender&& sender, t_Algorithm&& algo) {
            return algo(std::forward<InlineSchedulerSender>(sender));
        }
    };
}

namespace saturn {
    struct InlineScheduler {
        static auto schedule() {
            return inline_scheduler_detail::InlineSchedulerSender();
        }

        bool operator == (const InlineScheduler&) const = default;
    };
}

#endif
