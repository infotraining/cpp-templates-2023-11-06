#include <algorithm>
#include <cassert>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <type_traits>

using namespace std;

namespace vt
{
    namespace HeadTail
    {
        template <typename THead>
        auto sum(THead head)
        {
            return head;
        }

        template <typename THead, typename... TTail>
        auto sum(THead head, TTail... tail)
        {
            return head + sum(tail...);
        }
    } // namespace HeadTail

    template <typename... Ts>
    auto sum(Ts... values)
    {
        return (... + values); // ((1 + 3) + 3) - left fold
    }

    template <typename... Ts>
    auto sum_r(Ts... values)
    {
        return (values + ...); // ((1 + (3 + 3)) - right fold
    }

} // namespace vt

TEST_CASE("variadic sum")
{
    SECTION("for ints")
    {
        auto sum = vt::sum(1, 3, 3);

        REQUIRE(sum == 7);
        static_assert(is_same<int, decltype(sum)>::value, "Error");
    }

    SECTION("for floating points")
    {
        auto dbl_sum = vt::sum(1.1, 3.0f, 3);

        REQUIRE(dbl_sum == Catch::Approx(7.1));
        static_assert(is_same<double, decltype(dbl_sum)>::value, "Error");
    }

    SECTION("for strings")
    {
        auto text = vt::sum("Hello", string("world"), "!");

        REQUIRE(text == "Helloworld!");
        static_assert(is_same<string, decltype(text)>::value, "Error");
    }
}

///////////////////////////////////////////////

namespace vt
{
    template <size_t... Indexes, typename Tpl>
    auto select(const Tpl& tpl)
    {
        return std::tuple{std::get<Indexes>(tpl)...};
    }
} // namespace vt

TEST_CASE("select for tuple")
{
    std::tuple<int, std::string, std::string, std::vector<int>> row{1, "first-name", "last-name", std::vector<int>{1, 2, 3}};

    REQUIRE(vt::select<0, 2, 3>(row) == std::make_tuple(1, "last-name", std::vector{1, 2, 3}));
    REQUIRE(vt::select<0, 0, 0>(row) == std::tuple(1, 1, 1));
    REQUIRE(vt::select<3, 2, 1, 0>(row) == std::make_tuple(std::vector{1, 2, 3}, "last-name", "first-name", 1));
}

/////////////////////////////////////////////////

namespace vt
{
    // TODO - implement call_n_times function that calls given function n times
    template <size_t Count, typename F, typename... Args>
    void call_n_times(const F& func, const Args&... args)
    {
        for (size_t i = 0; i < Count; i++)
            func(args...);
    }
} // namespace vt

TEST_CASE("call_n_times wrapper")
{
    int counter{};
    std::vector<std::tuple<int, std::string>> results;

    auto func = [&counter, &results](auto&&... args) {
        ++counter;
        results.emplace_back(std::forward<decltype(args)>(args)...);
    };

    // vt::call_n_times(5, func, 1, "one"s); // TODO - it should call 5 times func function with the following argument (5 x func(1, "one"s))
    vt::call_n_times<5>(func, 1, "one"s); // TODO - it should call 5 times func function with the following argument (5 x func(1, "one"s))

    REQUIRE(counter == 5);
    REQUIRE(results.size() == 5);
    REQUIRE(std::all_of(begin(results), end(results), [](const auto& item) { return item == std::make_tuple(1, "one"s); }));

    constexpr size_t N = 10;

    using Indexes = std::make_index_sequence<N>;
}

///////////////////////////////////////////////

namespace vt
{
    // TODO - make_vector function that creates vector from a list of arguments
    // Tip: use std::common_type_t<Ts...> trait
    template <typename... T>
    auto make_vector(T&&... args)
    {
        std::vector<std::common_type_t<T...>> vec;
        vec.reserve(sizeof...(args));

        (vec.push_back(std::forward<T>(args)), ...);
        return vec;
    }
} // namespace vt

TEST_CASE("make_vector - create vector from a list of arguments")
{
    // const std::vector<std::unique_ptr<int>> vec = { std::make_unique<int>(13) };

    SECTION("ints")
    {
        std::vector<double> v = vt::make_vector(1, 2.0, 3);

        REQUIRE(v == vector{1.0, 2.0, 3.0});
    }

    SECTION("unique_ptrs")
    {
        auto ptrs = vt::make_vector(make_unique<int>(5), make_unique<int>(6));

        REQUIRE(ptrs.size() == 2);
        REQUIRE(*ptrs[0] == 5);
        REQUIRE(*ptrs[1] == 6);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

namespace vt
{
    template <typename T>
    concept Hashable = requires(T obj) {
        {
            std::hash<T>{}(obj)
        } -> std::convertible_to<size_t>;
    };

    template <Hashable T>
    void hash_combine(uint32_t& seed, const T& value)
    {
        seed ^= hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template <Hashable... T>
    auto combined_hash(const T&... args)
    {
        uint32_t seed{0};
        (hash_combine(seed, args), ...);
        return seed;
    }
} // namespace vt

TEST_CASE("combined_hash - write a function that calculates combined hash value for a given number of arguments")
{
#ifdef _MSC_VER
    CHECK(vt::combined_hash(1U) == 2577477565);
    CHECK(vt::combined_hash(1, 3.14, "string"s) == 2865319663);
    CHECK(vt::combined_hash(123L, "abc"sv, 234, 3.14f) == 1326321790);
#else
    CHECK(combined_hash(1U) == 2654435770U);
    CHECK(combined_hash(1, 3.14, "string"s) == 10365827363824479057U);
    CHECK(combined_hash(123L, "abc"sv, 234, 3.14f) == 162170636579575197U);
#endif
}

/////////////////////////////////////////////////////////////////////
// index_sequence

namespace vt
{
    namespace details
    {
        template <typename F, typename Tpl, size_t... Indexes>
        void tuple_apply_impl(F&& f, const Tpl& tpl, std::index_sequence<Indexes...> is)
        {
            (..., f(std::get<Indexes>(tpl)));
        }
    } // namespace details

    template <typename F, typename... Ts>
    void tuple_apply(F&& f, const std::tuple<Ts...>& tpl)
    {
        details::tuple_apply_impl(std::forward<F>(f), tpl, std::make_index_sequence<sizeof...(Ts)>{});
    }
}

TEST_CASE("tuple_apply")
{
    std::tuple tpl{1, 3.14, "text"s};

    vt::tuple_apply([](const auto& item) { std::cout << "item: " << item << "\n"; }, tpl);
}