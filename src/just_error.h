#ifndef SATURN_JUST_ERROR_H
#define SATURN_JUST_ERROR_H

#include <utility>
#include <stdexcept>

namespace saturn::just_error_detail {
    template <typename R>
    struct JustErrorOperation {
        std::exception_ptr m_Exception;
        R                  m_Receiver;

        void start() noexcept {
            m_Receiver.set_error(m_Exception);
        }
    };

    struct JustErrorSender {
        using result_t = void;

        std::exception_ptr m_Exception;

        template <typename R>
        auto connect(R&& recv)
            -> JustErrorOperation<R>
        {
            return JustErrorOperation<R>(
                m_Exception,
                std::forward<R>(recv)
            );
        }

        template <typename t_Algorithm>
        friend auto operator | (JustErrorSender&& self, t_Algorithm&& algorithm) {
            return algorithm(std::forward<JustErrorSender>(self));
        }
    };
}

namespace saturn {
    inline auto just_error(const std::exception_ptr& err)
        -> just_error_detail::JustErrorSender
    {
        return {
            err
        };
    }
}

#endif
