#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>

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

    std::integral auto result1 = max_value(&x, &y);                // std::integral<decltype(result)>
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
concept BigType = requires {
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
    {
        std::hash<T>{}(obj)
    } -> std::convertible_to<uint64_t>;
};

static_assert(Hashable<std::string>);

std::integral auto multiply(std::integral auto x, std::integral auto y)
    requires std::same_as<decltype(x), decltype(y)>
{
    return x * y;
}

namespace Alt
{
    template <typename T>
        requires std::integral<T>
    std::integral auto multiply(T x, T y)
    {
        return x * y;
    }

    namespace Shorter
    {
        template <std::integral T>
        std::integral auto multiply(T x, T y)
        {
            return x * y;
        }
    } // namespace Shorter
} // namespace Alt

///////////////////////////////////////////////////////////////////
// Subsumation

struct BoundingBox
{
    int w, h;
};

struct Color
{
    int r, g, b;
};

template <typename T>
concept Shape = requires(T obj) {
    { obj.box() } -> std::same_as<BoundingBox>;
    obj.draw();
};

template <typename T>
concept ShapeWithColor = Shape<T> && requires(T obj, Color c) {
    obj.set_color(c);
    { obj.get_color() } -> std::convertible_to<Color>;
};

template <typename T>
concept ShapeWithShading = ShapeWithColor<T> && requires(T obj, Color c) {
    obj.shade();
};

struct Rect
{
    int width, height;
    Color color{};

    BoundingBox box() const
    {
        return {width, height};
    }

    void draw() const
    {
        std::cout << "Drawing Rect: " << width << ", " << height << "\n";
    }

    void set_color(Color new_color)
    {
        std::cout << "Setting color...\n";
        color = new_color;
    }

    Color get_color() const
    {
        return color;
    }

    void shade() 
    {
        std::cout << "Shading rect...\n";
    }
};

static_assert(Shape<Rect>);
static_assert(ShapeWithColor<Rect>);

void render(Shape auto shp)
{
    shp.draw();
}

void render(ShapeWithColor auto shp)
{
    shp.set_color(Color{0, 0, 0});
    shp.draw();
}

void render(ShapeWithShading auto shp)
{
    shp.shade();
    shp.draw();
}

TEST_CASE("subsuming concepts")
{
    Rect rect{10, 20};
    render(rect);
}

void my_sort(std::ranges::random_access_range auto container)
{
    std::sort(container.begin(), container.end());
}

void my_sort(std::ranges::bidirectional_range auto container)
{
    container.sort();
}

TEST_CASE("sorting & subsuming")
{
    std::vector vec = {42, 1, 665};
    my_sort(vec);

    std::list lst = {42, 1, 665};
    my_sort(lst);
}