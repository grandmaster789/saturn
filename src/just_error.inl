#ifndef SATURN_JUST_ERROR_INL
#define SATURN_JUST_ERROR_INL

#include "just_error.h"

namespace saturn::just_error_detail {
    template <typename t_Receiver>
    void JustErrorOperation<t_Receiver>::start() noexcept {
        m_Receiver.set_error(m_Exception);
    }

    template <typename t_Receiver>
    auto JustErrorSender::connect(t_Receiver&& recv)
        -> JustErrorOperation<t_Receiver>
    {
        return { m_Exception, std::forward<t_Receiver>(recv) };
    }
}

namespace saturn {
    inline auto just_error(const std::exception_ptr& exception)
        -> just_error_detail::JustErrorSender
    {
        return { exception };
    }
}

#endif
