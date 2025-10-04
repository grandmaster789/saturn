#ifndef SATURN_JUST_STOPPED_INL
#define SATURN_JUST_STOPPED_INL

#include "just_stopped.h"

namespace saturn::just_stopped_detail {
    template <typename t_Receiver>
    void JustStoppedOperation<t_Receiver>::start() noexcept {
        m_Receiver.set_stopped();
    }

    template <typename t_Receiver>
    auto JustStoppedSender::connect(t_Receiver&& recv)
        -> JustStoppedOperation<t_Receiver>
    {
        return {
            std::forward<t_Receiver>(recv)
        };
    }
}

namespace saturn {
    inline auto just_stopped()
            -> just_stopped_detail::JustStoppedSender
    {
        return {};
    }
}

#endif
