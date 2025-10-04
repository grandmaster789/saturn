#ifndef SATURN_JUST_STOPPED_H
#define SATURN_JUST_STOPPED_H

#include <utility>
#include <stdexcept>

namespace saturn::just_stopped_detail {
    template <typename t_Receiver>
    struct JustStoppedOperation {
        t_Receiver m_Receiver;

        void start() noexcept;
    };

    struct JustStoppedSender {
        using result_t = void;

        template <typename t_Receiver>
        auto connect(t_Receiver&& recv)
            -> JustStoppedOperation<t_Receiver>;

        template <typename t_Algorithm>
        friend auto operator | (JustStoppedSender&& self, t_Algorithm&& algorithm) {
            return algorithm(std::forward<JustStoppedSender>(self));
        }
    };
}

namespace saturn {
    inline auto just_stopped()
        -> just_stopped_detail::JustStoppedSender;
}

#include "just_stopped.inl"

#endif
