#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

template <typename T>
void foo(T obj)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

template <typename T>
void foo(T* ptr)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

TEST_CASE("SFINAE")
{
    foo(42);
}

/////////////////////////////////////////////////////////////////

template <bool Condition, typename T = void>
struct EnableIf
{
    using type = T;
};

template <typename T>
struct EnableIf<false, T>
{ };

template <bool Condition, typename T = void>
using EnableIf_t = typename EnableIf<Condition, T>::type;

static_assert(std::is_same_v<EnableIf<true>::type, void>);
static_assert(std::is_same_v<EnableIf_t<true, int>, int>);
// static_assert(std::is_same_v<EnableIf<false, int>::type, int>);

////////////////////////////////////////////////////////////////////////

template <typename T>
auto do_stuff(T obj) -> std::enable_if_t<(sizeof(T) <= 8)> // for small objects
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

template <typename T>
auto do_stuff(const T& obj) -> std::enable_if_t<(sizeof(T) > 8)> // for big objects
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

namespace AlternativeTake
{
    template <typename T, typename = std::enable_if_t<(sizeof(T) <= 8)>>
    void do_stuff(T obj)  // for small objects
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
    }

    template <typename T, typename = void, typename = std::enable_if_t<(sizeof(T) > 8)>>
    void do_stuff(const T& obj)
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
    }
} // namespace AlternativeTake

TEST_CASE("SFINAE + enable_if")
{
    AlternativeTake::do_stuff(42);

    std::vector<int> vec = {1, 2, 3, 4};

    AlternativeTake::do_stuff(vec);
}