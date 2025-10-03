#ifndef SATURN_VOID_FILTER_H
#define SATURN_VOID_FILTER_H

#include <tuple>

namespace saturn {
    template <typename... Ts>
    struct TupleVoidFilter;

    template <>
    struct TupleVoidFilter<> {
        using type = std::tuple<>;
    };

    template <typename t_Head, typename...t_Tail>
    struct TupleVoidFilter<t_Head, t_Tail...> {
        using tail_filtered_t = typename TupleVoidFilter<t_Tail...>::type;
        using type = std::conditional_t<
            std::is_void_v<t_Head>,
            tail_filtered_t, // if the head is a void, just append the rest of the sequence
            // if not, use tuple_cat to create a list with the head included
            decltype(
                std::tuple_cat(
                    std::declval<std::tuple<t_Head>>(),
                    std::declval<tail_filtered_t>()
                )
            )
        >;
    };

    template <typename... Ts>
    using void_filtered_tuple_t = typename TupleVoidFilter<Ts...>::type;

    // ------------------------------------------------------------------------------------------
    template <std::size_t t_Index, typename... Ts>
    struct IndexVoidFilter;;

    template <std::size_t t_Index>
    struct IndexVoidFilter<t_Index> {
        static constexpr std::size_t value = 0;
    };

    template <std::size_t t_Index, typename t_Head, typename... t_Tail>
    struct IndexVoidFilter<t_Index, t_Head, t_Tail...> {
        static constexpr std::size_t value =
            (t_Index == 0) ? 0 :
            std::is_void_v<t_Head> ?
                IndexVoidFilter<t_Index - 1, t_Tail...>::value :
                IndexVoidFilter<t_Index - 1, t_Tail...>::value + 1;
    };
}

#endif
