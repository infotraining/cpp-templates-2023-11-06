#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

namespace ver_1
{
    template <typename T>
    auto max_value(T a, T b)
    {
        return a < b ? b : a;
    }

    template <typename T>
        requires std::is_pointer_v<T> // constraint
    auto max_value(T a, T b)
    {
        assert(a != nullptr);
        assert(b != nullptr);
        return *a < *b ? *b : *a;
    }
} // namespace ver_1

// template <typename T>
// concept Pointer = std::is_pointer_v<T>;

template <typename T>
concept Pointer = requires(T ptr) {
    *ptr;
    ptr == nullptr;
    ptr != nullptr;
};

namespace ver_2
{
    template <typename T>
    auto max_value(T a, T b)
    {
        return a < b ? b : a;
    }

    template <typename T>
        requires Pointer<T> // constraint
    auto max_value(T a, T b)
    {
        assert(a != nullptr); // !(a == nullptr)
        assert(b != nullptr);
        return *a < *b ? *b : *a;
    }
} // namespace ver_2

namespace ver_3
{
    template <typename T>
    auto max_value(T a, T b)
    {
        return a < b ? b : a;
    }

    template <Pointer T> // constraint - Pointer<T> must be true
    auto max_value(T a, T b)
    {
        assert(a != nullptr);
        assert(b != nullptr);
        return max_value(*a, *b);
    }
} // namespace ver_3

namespace ver_4
{
    auto max_value(auto a, auto b)
    {
        return a < b ? b : a;
    }

    auto max_value(Pointer auto a, Pointer auto b)
    {
        assert(a != nullptr);
        assert(b != nullptr);
        return max_value(*a, *b);
    }
} // namespace ver_4

static_assert(Pointer<int*>);
static_assert(Pointer<int> == false);

TEST_CASE("concepts")
{
    using namespace ver_4;

    CHECK(max_value(42, 665) == 665);
    CHECK(max_value("text"s, "test"s) == "text"s);

    int x = -10;
    int y = 20;

    std::integral auto result1 = max_value(&x, &y); // std::integral<decltype(result)>
    std::convertible_to<int64_t> auto result2 = max_value(&x, &y); // std::convertible_to<decltype(result), int64_t>
    CHECK(result1 == result2);

    std::integral auto result3 = max_value(std::make_shared<int>(42), std::make_shared<int>(665));
}

template <typename T>
struct Wrapper
{
    T value;

    void print() const
    {
        std::cout << "value: " << value << "\n";
    }

    void print() const
        requires std::ranges::range<T>
    {
        std::cout << "values: ";
        for (const auto& item : value)
            std::cout << item << " ";
        std::cout << "\n";
    }
};

TEST_CASE("Wrapper with print")
{
    Wrapper w1{42};
    w1.print();

    Wrapper w2{std::vector{1, 2, 3}};
    w2.print();
}

/////////////////////////////////////////////////////////////////////
// requires expression

template <typename T>
concept Addable = requires(T a, T b) {
    a + b; // simple requirement
};

template <typename T>
concept BigType = requires 
{
    requires sizeof(T) > 8; // now evaluated to bool
};

static_assert(BigType<char> == false);
static_assert(BigType<std::string>);

template <typename T>
concept HasValueType = requires {
    typename T::value_type;
};

static_assert(HasValueType<std::vector<int>>);
static_assert(HasValueType<int[10]> == false);

template <typename T>
concept Hashable = requires(T obj) {
    { std::hash<T>{}(obj) } -> std::convertible_to<uint64_t>;
};

static_assert(Hashable<std::string>);