#ifndef SATURN_JUST_STOPPED_H
#define SATURN_JUST_STOPPED_H

#include <utility>
#include <stdexcept>

namespace saturn::just_stopped_detail {
    template <typename R>
    struct JustStoppedOperation {
        R m_Receiver;

        void start() noexcept {
            m_Receiver.set_stopped();
        }
    };

    struct JustStoppedSender {
        using result_t = void;

        template <typename R>
        auto connect(R&& recv)
            -> JustStoppedOperation<R>
        {
            return {
                std::forward<R>(recv)
            };
        }

        template <typename t_Algorithm>
        friend auto operator | (JustStoppedSender&& self, t_Algorithm&& algorithm) {
            return algorithm(std::forward<JustStoppedSender>(self));
        }
    };
}

namespace saturn {
    inline auto just_stopped()
        -> just_stopped_detail::JustStoppedSender
    {
        return {};
    }
}

#endif
