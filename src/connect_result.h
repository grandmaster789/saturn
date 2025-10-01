#ifndef SATURN_CONNECT_RESULT_H
#define SATURN_CONNECT_RESULT_H

#include <utility>

namespace saturn {
    template <
        typename t_Sender,
        typename t_Receiver
    >
    using connect_result_t = decltype(
        std::declval<std::decay_t<t_Sender>>()
            .connect(std::declval<std::decay_t<t_Receiver>>())
    );
}

#endif