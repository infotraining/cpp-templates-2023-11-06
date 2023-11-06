#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <numbers>

using namespace std::literals;

template <typename T, size_t N>
struct Array
{
    T items[N];

    typedef T* iterator;
    using const_iterator = const T*;
    using reference = T&;
    using const_reference = const T&;

    constexpr size_t size() const
    {
        return N;
    }

    iterator begin()
    {
        return items;
    }

    iterator end()
    {
        return items + N;
    }

    const_iterator begin() const
    {
        return items;
    }

    const_iterator end() const
    {
        return items + N;
    }

    reference operator[](size_t index)
    {
        return items[index];
    }

    const_reference operator[](size_t index) const
    {
        return items[index];
    }

    reference at(size_t index);

    const_reference at(size_t index) const
    {
        if (index >= N)
            throw std::out_of_range("index out of range");

        return items[index];
    }
};

// deduction guide
template <typename T, typename... Ts>
Array(T, Ts...) -> Array<T, sizeof...(Ts) + 1>;

template <typename T, size_t N>
typename Array<T, N>::reference Array<T, N>::at(size_t index)
{
    if (index >= N)
        throw std::out_of_range("index out of range");

    return items[index];
}

TEST_CASE("class templates")
{
    Array<int, 10> arr1 = {1, 2, 3, 4};

    for (const auto& item : arr1)
        std::cout << item << " ";
    std::cout << "\n";

    REQUIRE_THROWS_AS(arr1.at(11), std::out_of_range);

    Array arr2 = {3.14, 6.65, 0.14}; // Array<double, 3>
}

////////////////////////////////////////////////////////////

template <typename T1, typename T2>
struct Pair
{
    T1 first;
    T2 second;

    // template <typename U1, typename U2>
    // Pair(U1&& f, U2&& s)
    //     : first{std::forward<U1>(f)}
    //     , second{std::forward<U2>(s)}
    //{
    // }

    template <typename U1, typename U2>
    Pair(U1&& f, U2&& s);
};

template <typename T1, typename T2>
template <typename U1, typename U2>
Pair<T1, T2>::Pair(U1&& f, U2&& s)
    : first{std::forward<U1>(f)}
    , second{std::forward<U2>(s)}
{
}

// deduction guide
template <typename T1, typename T2>
Pair(T1, T2) -> Pair<T1, T2>;

template <typename T1, typename T2>
Pair<T1, T2> my_make_pair(T1 f, T2 s)
{
    return Pair<T1, T2>(f, s);
}

TEST_CASE("Pair")
{
    Pair<int, double> p1{1, 3.14};

    p1 = my_make_pair(4, 6.66);

    CHECK(p1.first == 4);
    CHECK(p1.second == 6.66);

    // Class-Template Argument Deduction - CTAD
    Pair p3{1, 4.44};         // Pair<int, double>
    Pair p4{"text", "text"s}; // Pair<const char*, std::string> p4
}

// partial specialization
template <typename T>
struct Pair<T, T>
{
    T first, second;

    template <typename U1, typename U2>
    Pair(U1&& f, U2&& s)
        : first{std::forward<U1>(f)}
        , second{std::forward<U2>(s)}
    {
    }

    const T& max_value() const
    {
        return first < second ? second : first;
    }
};

TEST_CASE("partial specialization")
{
    Pair<int, int> p1(42, 665);
    CHECK(p1.max_value() == 665);

    Pair p2{3.14, 66.5};
    CHECK(p2.max_value() == 66.5);
}

// full specialization
template <>
struct Pair<const char*, const char*>
{
    std::string first;
    std::string second;

    Pair(const std::string& f, const std::string& s)
        : first{f}
        , second{s}
    {
    }

    const std::string& max_value() const
    { 
        return first < second ? second : first;
    }
};

TEST_CASE("full specialization")
{
    Pair p1{"ala", "ola"};
    static_assert(std::is_same_v<decltype(p1.first), std::string>);

    CHECK(p1.max_value() == "ola");
}

TEST_CASE("vector<bool>")
{ 
    std::vector<bool> flags = {0, 1, 1, 0};

    flags[0] = true;

    flags.flip();
}

///////////////////////////////////////////////////////////
// alias template

template <typename T>
using Dictionary = std::map<std::string, T>;

TEST_CASE("alias templates")
{
    Dictionary<int> dict1 = {{"one", 1}, {"two", 2}, {"three", 3}};
}

///////////////////////////////////////////////////////////
// template  variables

template <typename T>
constexpr T pi(3.14159265359);

TEST_CASE("template variables")
{
    std::cout << "pi<double>:" << pi<double> << "\n";
    std::cout << "pi<float>:" << pi<float> << "\n";
    
    std::cout << "std::pi_v<double>: " << std::numbers::pi_v<double> << "\n";
}