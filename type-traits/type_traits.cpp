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

template<typename T>
struct RemoveRef
{
    using type = T;
};

template<typename T>
struct RemoveRef<T&>
{
    using type = T;
};

template <typename T>
struct RemoveRef<T&&>
{
    using type = T;
};

template<typename T>
using RemoveRef_t = typename RemoveRef<T>::type;

TEST_CASE("type traits - RemoveRef")
{
    static_assert(std::is_same_v<RemoveRef_t<int&>, int>);
    static_assert(std::is_same_v<RemoveRef_t<const std::vector<int>&>, const std::vector<int>>);
    static_assert(std::is_same_v<RemoveRef_t<std::vector<int>&&>, std::vector<int>>);

    using T = int&;

    RemoveRef_t<T> obj{};  // int obj;
}

//////////////////////////////////////////////////////////////////

template <typename T, T v>
struct IntegralConstant 
{
    static constexpr T value = v;
};

template <typename T, T v>
constexpr T IntegralConstant_v = IntegralConstant<T, v>::value;

TEST_CASE("type traits - IntegralConstant")
{
    static_assert(IntegralConstant<int, 5>::value == 5);
    static_assert(IntegralConstant_v<int, 5> == 5);
}

template <bool v>
using BoolConstant = IntegralConstant<bool, v>;

using TrueType = BoolConstant<true>;
using FalseType = BoolConstant<false>;

////////////////////////////////////////////////////////////////////

template <typename T>
struct IsVoid : std::false_type
{
    //static constexpr bool value = false;
};

template <>
struct IsVoid<void> : std::true_type
{
    //static constexpr bool value = true;
};

template <typename T>
constexpr bool IsVoid_v = IsVoid<T>::value;

TEST_CASE("type trait - IsVoid")
{
    static_assert(IsVoid<int>::value == false);
    static_assert(IsVoid_v<void> == true);
}

///////////////////////////////////////////////////////////////////

template <typename T>
struct IsPointer : std::false_type{};
 
template <typename T>
struct IsPointer<T*> : std::true_type{};
 
template <typename T>
constexpr bool IsPointer_v = IsPointer<T>::value;
 
static_assert(IsPointer<int*>::value);
static_assert(IsPointer_v<const int*>);
static_assert(IsPointer_v<const int> == false);