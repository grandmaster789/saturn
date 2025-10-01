#ifndef SATURN_CONCEPTS_H
#define SATURN_CONCEPTS_H

#include <concepts>
#include <stdexcept>

namespace saturn {
    template <typename R, typename T>
    concept c_Receiver = requires(R receiver, T value) {
        { receiver.set_value(value)                } -> std::same_as<void>;
        { receiver.set_value()                     } -> std::same_as<void>;
        { receiver.set_error(std::exception_ptr()) } -> std::same_as<void>;
        { receiver.set_stopped()                   } -> std::same_as<void>;
    };

    template <typename O>
    concept c_OperationState = requires(O operation) {
        { operation.start() } -> std::same_as<void>;
    };

    template <typename S, typename R>
    concept c_Sender = requires(S sender, R recv) {
        { sender.connect(recv) } -> c_OperationState; // should have a template to connect with any receiver, resulting in an operation state
        { sender.result_t      } -> std::same_as<typename S::result_t>; // should declare a result_t
    };
}

#endif
