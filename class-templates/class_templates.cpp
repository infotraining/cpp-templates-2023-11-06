#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

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

    const_reference at(size_t index) const;
    {
        if (index >= N)
            throw std::out_of_range("index out of range");

        return items[index];
    }
};

template<typename T, size_t N>
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
}