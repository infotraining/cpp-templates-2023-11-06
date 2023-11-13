#include <array>
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
    void do_stuff(T obj) // for small objects
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
    }

    template <typename T, typename = void, typename = std::enable_if_t<(sizeof(T) > 8 && sizeof(T) <= 16)>>
    void do_stuff(const T& obj)
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
    }

    template <typename T, typename = void, typename = std::enable_if_t<(sizeof(T) > 16)>>
    void do_stuff(std::shared_ptr<T> ptr)
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
    }
} // namespace AlternativeTake

struct Data
{
    int values[100] = {};

    Data() = default;
};

TEST_CASE("SFINAE + enable_if")
{
    AlternativeTake::do_stuff(42);

    std::vector<int> vec = {1, 2, 3, 4};

    AlternativeTake::do_stuff(vec);

    AlternativeTake::do_stuff(std::make_shared<Data>());
}

/////////////////////////////////////////////////////////////////////////

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool is_power_of_2(T value)
{
    return value > 0 && (value & (value - 1)) == 0;
}

template <typename T>
auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point_v<T>, bool>
{
    int exponent;
    const T mantissa = std::frexp(value, &exponent);
    return mantissa == T(0.5);
}

TEST_CASE("is power of 2")
{
    CHECK(is_power_of_2(512));
    CHECK(is_power_of_2(4u));
    CHECK(is_power_of_2(9ull) == false);

    CHECK(is_power_of_2(16.0));
    CHECK(is_power_of_2(512.0f));
}

///////////////////////////////////////////////////////////////////////////

template <typename T, typename Enabled = void>
struct DataSet
{
    std::array<T, 64> data = {};

    void info() const
    {
        std::cout << "Data with std::array!\n";
    }
};

template <typename T>
struct DataSet<T, std::enable_if_t<std::is_floating_point_v<T>>>
{
    std::vector<T> data = {};

    void info() const
    {
        std::cout << "Data with std::vector!\n";
    }
};

TEST_CASE("Data + enable_if")
{
    DataSet<int> d1;
    d1.info();

    DataSet<double> d2;
    d2.info();
}

////////////////////////////////////////////////////////////////////////////////////////

template <typename, typename = void>
constexpr bool is_iterable{}; // false

template <typename T>
constexpr bool is_iterable<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>>
    = true;


template <typename T, typename = std::enable_if_t<!is_iterable<T>>>
void print(const T& value)
{
    std::cout << "value: " << value << "\n";   
}

template <typename T, typename = void, typename = std::enable_if_t<is_iterable<T>>>
void print(const T& rng)
{
    std::cout << "values: ";
    for(const auto& item : rng)
        std::cout << item << " ";
    std::cout << "\n";
}

TEST_CASE("print")
{
    print(42);

    print(std::vector{1, 2, 3});
}