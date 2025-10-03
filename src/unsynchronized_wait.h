#ifndef SATURN_UNSYNCHRONIZED_WAIT_H
#define SATURN_UNSYNCHRONIZED_WAIT_H

#include <optional>
#include <exception>
#include <tuple>

namespace saturn::unsynchronized_wait_detail {
    struct UnsynchronizedWaitControlBlock {
        std::exception_ptr m_Exception;
        bool               m_Completed = false;
    };

    template <typename T>
    struct UnsynchronizedWaitReceiver {
        UnsynchronizedWaitControlBlock& m_ControlBlock;
        std::optional<T>&               m_Result;

        template <typename... Us>
        void set_value(Us&&... values) {
            if constexpr (sizeof...(Us) == 1) {
                m_Result.emplace(std::forward<Us>(values)...);
            }
            else {
                m_Result.emplace(std::make_tuple(std::forward<Us>(values)...));
            }

            m_ControlBlock.m_Completed = true;
        }

        void set_error(const std::exception_ptr& ptr) {
            m_ControlBlock.m_Exception = ptr;
            m_ControlBlock.m_Completed = true;
        }

        void set_stopped() {
            m_ControlBlock.m_Completed = true;
        }
    };

    template <>
    struct UnsynchronizedWaitReceiver<void> {
        UnsynchronizedWaitControlBlock& m_ControlBlock;

        void set_value() {
            m_ControlBlock.m_Completed = true;
        }

        void set_error(const std::exception_ptr& ptr) {
            m_ControlBlock.m_Exception = ptr;
            m_ControlBlock.m_Completed = true;
        }

        void set_stopped() {
            m_ControlBlock.m_Completed = true;
        }
    };

    struct UnsynchronizedWait {
        template <typename S>
        auto operator()(S&& sender) const {
            using T = std::remove_cvref_t<S>::result_t;

            UnsynchronizedWaitControlBlock ctl;

            if constexpr (std::is_void_v<T>) {
                auto state = sender.connect(UnsynchronizedWaitReceiver<void> { ctl });
                state.start();

                if (ctl.m_Exception)
                    std::rethrow_exception(ctl.m_Exception);

                // could be either stopped or completed
            }
            else {
                std::optional<T> result;

                auto state = sender.connect(UnsynchronizedWaitReceiver<T> { ctl, result });
                state.start();

                if (ctl.m_Exception)
                    std::rethrow_exception(ctl.m_Exception);

                return result; // if the operation was stopped, the result will be empty
            }
        }

        template <typename S>
        friend auto operator | (S&& sender, const UnsynchronizedWait&) {
            return UnsynchronizedWait()(std::forward<S>(sender));
        }
    };
}

namespace saturn {
    // this way we support both direct calls and pipe syntax
    inline constexpr unsynchronized_wait_detail::UnsynchronizedWait unsynchronized_wait{};
}

#endif
