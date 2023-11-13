#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

template <typename T>
struct Identity
{
    using type = T;
};

template <typename T>
using Identity_t = typename Identity<T>::type;

TEST_CASE("type traits - Identity")
{
    static_assert(std::is_same_v<Identity<int>::type, int>);
    static_assert(std::is_same_v<Identity_t<int>, int>);
}

/////////////////////////////////////////////////////////////

template <typename T>
struct RemoveConst
{
    using type = T;
};

template <typename T>
struct RemoveConst<const T>
{
    using type = T;
};

template <typename T>
using RemoveConst_t = typename RemoveConst<T>::type;

TEST_CASE("type traits - RemoveConst")
{
    using T = const int;

    RemoveConst_t<T> obj{};  // int obj;
    ++obj;
}

////////////////////////////////////////////////////////////////

// TODO

TEST_CASE("type traits - RemoveRef")
{
    static_assert(std::is_same_v<RemoveRef_t<int&>, int>);
    static_assert(std::is_same_v<RemoveRef_t<const std::vector<int>&, const std::vector<int>>);
    static_assert(std::is_same_v<RemoveRef_t<std::vector<int>&&, std::vector<int>>);
}