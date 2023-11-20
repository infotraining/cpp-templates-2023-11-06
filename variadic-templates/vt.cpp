#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using namespace std::literals;

namespace VT
{
    template <typename... Types>
    struct Row
    {
        std::tuple<Types...> data;
    };

    template <typename... Types>
    struct Pointers
    {
        std::tuple<const Types*...> pointers;
    };

    template <typename... TArgs>
    void eat_anything(TArgs&&... args)
    {
    }
} // namespace VT

TEST_CASE("vt")
{
    VT::Row<int, double, std::string> r1;
    VT::Row<> r2;

    VT::eat_anything(1, 3.14, "text"s);
    VT::eat_anything();
}

///////////////////////////////////////////////////////////////////
// idiom Head-Tail
namespace VT
{
    // void print()
    // {
    //     std::cout << "\n";
    // }

    template <typename THead, typename... TTail>
    void print(const THead& head, const TTail&... tail)
    {
        std::cout << head << " ";

        if constexpr(sizeof...(tail) > 0)
        {
            print(tail...);
        }
        else
        {
            std::cout << "\n";
        }        
    }

    namespace Folds
    {
        void print(const auto&... values)
        {
            //(std::cout << ... << values) << "\n"; // binary left fold: (start << ... << pack)

            (..., (std::cout << values << " ")) << "\n"; // operator ,
        }
    }

    //////////////////////////////////////////////////////////

    template <typename... Ts>
    struct Count;

    template <typename THead, typename... TTail>
    struct Count<THead, TTail...>
    {
        static constexpr size_t value = 1 + Count<TTail...>::value;
    };

    template <>
    struct Count<>
    {
        static constexpr size_t value = 0;
    };

    namespace Shorter
    {
        template <typename... Ts>
        struct Count
        {
            constexpr static size_t value = sizeof...(Ts);
        };
    }
} // namespace VT

TEST_CASE("Head-Tail")
{
    VT::Folds::print(1, 2.3, "text"s);

    static_assert(VT::Count<int, double, std::string>::value == 3);
}

///////////////////////////////////////////////////////////

namespace VT
{
    template <size_t... Is>
    struct Indexes
    {};

    template <size_t... Is, typename Tpl>
    auto cherry_pick(const Tpl& tpl)
    {
        return std::tuple{std::get<Is>(tpl)...}; // std::tuple{std::get<0>(tpl), std::get<2>(tpl)};
    }
}

TEST_CASE("indexes")
{
    VT::Indexes<0, 1, 2, 3, 4, 5> index_seq{};

    std::tuple tpl{1, 3.14, "text"s, std::vector{1, 2, 3}};

    std::tuple picked_values = VT::cherry_pick<0, 2>(tpl);

    CHECK(picked_values == std::tuple{1, "text"s});
}