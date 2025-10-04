#ifndef SATURN_JUST_ERROR_H
#define SATURN_JUST_ERROR_H

#include <utility>
#include <stdexcept>

namespace saturn::just_error_detail {
    template <typename t_Receiver>
    struct JustErrorOperation {
        std::exception_ptr m_Exception;
        t_Receiver         m_Receiver;

        void start() noexcept;
    };

    struct JustErrorSender {
        using result_t = void;

        std::exception_ptr m_Exception;

        template <typename t_Receiver>
        auto connect(t_Receiver&& recv)
            -> JustErrorOperation<t_Receiver>;

        template <typename t_Algorithm>
        friend auto operator | (JustErrorSender&& self, t_Algorithm&& algorithm) {
            return algorithm(std::forward<JustErrorSender>(self));
        }
    };
}

namespace saturn {
    inline auto just_error(const std::exception_ptr& exception)
        -> just_error_detail::JustErrorSender;
}

#include "just_error.inl"

#endif
