#ifndef SATURN_SYNC_WAIT_H
#define SATURN_SYNC_WAIT_H

#include <mutex>
#include <condition_variable>
#include <optional>
#include <exception>

namespace saturn::sync_wait_detail {
    struct SyncWaitControlBlock {
        std::mutex              m_Mutex;
        std::condition_variable m_Condition;
        std::exception_ptr      m_Exception;
        bool                    m_Completed = false;
    };

    template <typename T>
    struct SyncWaitReceiver {
        SyncWaitControlBlock& m_ControlBlock;
        std::optional<T>&     m_Result;

        template <typename U>
        void set_value(U&& value) {
            std::unique_lock guard(m_ControlBlock.m_Mutex);

            m_Result.emplace(std::forward<U>(value));
            m_ControlBlock.m_Completed = true;
            m_ControlBlock.m_Condition.notify_one();
        }

        void set_error(std::exception_ptr exception) const {
            std::unique_lock guard(m_ControlBlock.m_Mutex);

            // transfer the exception to the control block
            m_ControlBlock.m_Exception = std::move(exception);
            m_ControlBlock.m_Completed = true;
            m_ControlBlock.m_Condition.notify_one();
        }

        void set_stopped() const {
            std::unique_lock guard(m_ControlBlock.m_Mutex);

            m_ControlBlock.m_Completed = true;
            m_ControlBlock.m_Condition.notify_one();
        }
    };

    template <>
    struct SyncWaitReceiver<void> {
        SyncWaitControlBlock& m_ControlBlock;

        void set_value() const {
            std::unique_lock guard(m_ControlBlock.m_Mutex);

            // with void result, we don't have an optional to
            m_ControlBlock.m_Completed = true;
            m_ControlBlock.m_Condition.notify_one();
        }

        void set_error(std::exception_ptr exception) const {
            std::unique_lock guard(m_ControlBlock.m_Mutex);

            // transfer the exception to the control block
            m_ControlBlock.m_Exception = std::move(exception);
            m_ControlBlock.m_Completed = true;
            m_ControlBlock.m_Condition.notify_one();
        }

        void set_stopped() const {
            std::unique_lock guard(m_ControlBlock.m_Mutex);

            m_ControlBlock.m_Completed = true;
            m_ControlBlock.m_Condition.notify_one();
        }
    };

    // sync_wait is a terminal algorithm; it can be added to the end of a pipeline (only at the end),
    // or it can be called directly
    struct SyncWait {
        // direct call
        template <typename S>
        auto operator()(S&& sender) const {
            using T = typename std::remove_cvref_t<S>::result_t;

            SyncWaitControlBlock ctl;

            if constexpr (std::is_void_v<T>) {
                auto state = sender.connect(SyncWaitReceiver<void> { ctl });
                state.start();

                std::unique_lock guard(ctl.m_Mutex);
                ctl.m_Condition.wait(guard, [&] { return ctl.m_Completed; });

                if (ctl.m_Exception)
                    std::rethrow_exception(ctl.m_Exception);

                // could be either stopped or completed
            }
            else {
                std::optional<T> result;

                auto state = sender.connect(SyncWaitReceiver<T> { ctl, result });
                state.start();

                std::unique_lock guard(ctl.m_Mutex);
                ctl.m_Condition.wait(guard, [&] { return ctl.m_Completed; });

                if (ctl.m_Exception)
                    std::rethrow_exception(ctl.m_Exception);

                return result; // if the operation was stopped, the result will be empty
            }
        }

        // pipeline call: sender | sync_wait
        template <typename S>
        friend auto operator | (S&& sender, const SyncWait&) {
            return SyncWait()(std::forward<S>(sender));
        }
    };
}

namespace saturn {
    // global instance
    inline constexpr sync_wait_detail::SyncWait sync_wait{};
}

#endif