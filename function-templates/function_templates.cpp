#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <list>

using namespace std::literals;

template <typename T>
T max_value(T a, T b)
{
    return a < b ? b : a;
}

template <>
const char* max_value(const char* txt1, const char* txt2)
{
    return std::strcmp(txt1, txt2) < 0 ? txt2 : txt1;
}

TEST_CASE("function templates")
{
    CHECK(max_value(4, 5) == 5); // max_value<int>(4, 5)
    
    CHECK(max_value(4.0, 4.14) == 4.14); // max_value<double>(4.0, 4.14)

    std::string str1 = "abc";
    std::string str2 = "def";
    CHECK(max_value(str1, str2) == "def"s); // max_value<std::string>(str1, str2);

    CHECK(max_value(4.0f, 4.14f) == 4.14f); // max_value<float>(4.0f, 4.14f);

    CHECK(max_value(static_cast<double>(4), 4.14) == 4.14); // cast
    CHECK(max_value<double>(4, 4.14) == 4.14); // turn-off deduction

    CHECK(max_value("abc", "def") == "def"s); 
    CHECK(max_value<const char*>("abc", "def") == "def"s);
}

namespace ver_2
{
    template <typename T1, typename T2>
    auto max_value(T1 a, T2 b)
    {
        return a < b ? b : a;
    }

    namespace Cpp20
    {
        auto max_value(auto a, auto b)
        { 
            return a < b ? b : a;
        }
    }
}

TEST_CASE("many params")
{
    CHECK(ver_2::max_value(42, 42.1) == 42.1);
}

namespace ver_3
{
    template <typename TResult, typename T1, typename T2>
    TResult max_value(T1 a, T2 b)
    {
        return a < b ? b : a;
    }
}

TEST_CASE("TResult")
{
    CHECK(ver_3::max_value<double>(42, 42.1) == 42.1);
}

template <>
struct std::common_type<float, float>
{
    using type = double;
};

namespace ver_4
{
    template <typename T1, typename T2>
    std::common_type_t<T1, T2> max_value(T1 a, T2 b)
    { 
        return a < b ? b : a;
    }
}

TEST_CASE("traits")
{
    CHECK(ver_4::max_value(42, 42.1) == 42.1);
    CHECK(ver_4::max_value("abc", "def"s) == "def"s);
}

namespace ver_5
{
    template <typename T1, typename T2, typename TResult = std::common_type_t<T1, T2>>
    TResult max_value(T1 a, T2 b)
    {
        return a < b ? b : a;
    }
}

TEST_CASE("default param")
{
    CHECK(ver_5::max_value(42, 42.1) == 42.1);
    CHECK(ver_5::max_value("abc", "def"s) == "def"s);

    CHECK(ver_5::max_value<float, int, double>(42.1, 42) == Catch::Approx(42.1));
}

TEST_CASE("tuned trait for float")
{
    auto result = ver_4::max_value(4.0f, 4.1f);

    static_assert(std::is_same_v<decltype(result), double>);
}

///////////////////////////////////////////////////////////////////////

namespace Training
{
    template <typename InputIter, typename Function>
    void foreach (InputIter first, InputIter last, Function f)
    {
        for (InputIter it = first; it != last; ++it)
            f(*it);
    }

    template <typename TContainer>
    auto begin(TContainer& container)
    {
        return container.begin();
    }

    template <typename TContainer>
    auto end(TContainer& container)
    {
        return container.end();
    }

    template <typename T, size_t N>
    auto begin(T (&tab)[N])
    {
        return &tab[0];
    }

    template <typename T, size_t N>
    auto end(T (&tab)[N])
    {
        return &tab[0] + N;
    }
}

TEST_CASE("foreach")
{
    int vec[] = {1, 2, 3, 4};

    Training::foreach(Training::begin(vec), Training::end(vec), [](int n) { std::cout << n << "\n"; });
}