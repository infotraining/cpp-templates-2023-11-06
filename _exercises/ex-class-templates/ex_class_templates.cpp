#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

using Catch::Matchers::Equals;

template <typename T, typename Container = std::deque<T>>
class Stack
{
public:
    using value_type = T;
    using reference = T&;
    using iterator = typename Container::const_iterator;
    using const_reference = const T&;
    using container_type = Container;
    using size_type = typename Container::size_type;

    Stack() = default;

    Stack(const Stack& other) = default;
    Stack& operator=(const Stack& other) = default;
    Stack(Stack&& other) = default;
    Stack& operator=(Stack&& other) = default;

    template <typename TOtherElement, typename TOtherContainer>
    friend class Stack;

    template <typename TOtherElement, typename TOtherContainer>
    Stack(const Stack<TOtherElement, TOtherContainer>& other);

    // void push(const value_type& value) // cc
    // {
    //     container_.push_back(value);
    // }

    // void push(value_type&& value) // mv
    // {
    //     container_.push_back(std::move(value));
    // }

    // template <typename TValue>
    // void push(TValue&& value)
    // {
    //     container_.push_back(std::forward<TValue>(value));
    // }

    void push(auto&& value)
    {
        container_.push_back(std::forward<decltype(value)>(value));
    }

    // template <typename... TArgs>
    // void emplace(TArgs&&... args)
    // {
    //     container_.emplace_back(std::forward<TArgs>(args)...);
    // }

    void emplace(auto&&... args)
    {
        container_.emplace_back(std::forward<decltype(args)>(args)...);
    }

    void pop(reference value)
    {
        if (container_.empty())
        {
            throw std::out_of_range("Stack is empty");
        }
        value = container_.back();
        container_.pop_back();
    }

    reference top()
    {
        if (container_.empty())
        {
            throw std::out_of_range("Stack is empty");
        }
        return container_.back();
    }

    const_reference top() const
    {
        if (container_.empty())
        {
            throw std::out_of_range("Stack is empty");
        }
        return container_.back();
    }

    bool empty() const noexcept
    {
        return container_.empty();
    }

    size_type size() const noexcept
    {
        return container_.size();
    }

private:
    Container container_;

    iterator begin() const
    {
        return std::begin(container_);
    }

    iterator end() const
    {
        return std::end(container_);
    }
};

template <typename T, typename Container>
template <typename TOtherElement, typename TOtherContainer>
Stack<T, Container>::Stack(const Stack<TOtherElement, TOtherContainer>& other)
    : container_{other.begin(), other.end()}
{
}

static_assert(std::is_same_v<Stack<int>::container_type, std::deque<int>>);
static_assert(std::is_same_v<Stack<int, std::vector<int>>::container_type, std::vector<int>>);

namespace TemplateAsTemplateParam
{
    template <typename T, template <typename V, typename Allocator> class Container = std::deque, typename TAllocator = std::allocator<T>>
    class Stack
    {
        Container<T, TAllocator> items_;

        template <typename U, template <typename V, typename Allocator> class UContainer, typename UAllocator>
        friend class Stack;

    public:
        using value_type = T;
        using reference = T&;
        using const_reference = const T&;
        using container_type = Container<T, TAllocator>;

        Stack() = default;

        template <std::convertible_to<T> U, template <typename V, typename Allocator> class UContainer, typename UAllocator>
        Stack(const Stack<U, UContainer, UAllocator>& other)
            : items_(other.items_.begin(), other.items_.end())
        {
        }

        bool empty() const
        {
            return items_.empty();
        }

        size_t size() const
        {
            return items_.size();
        }

        template <typename TItem>
        void push(TItem&& item)
            requires std::constructible_from<T, TItem>
        {
            items_.push_back(std::forward<TItem>(item));
        }

        const_reference top() const
        {
            return items_.back();
        }

        void pop(reference item)
        {
            item = std::move(items_.back());
            items_.pop_back();
        }
    };

    static_assert(std::is_same_v<Stack<int>::container_type, std::deque<int>>);
    static_assert(std::is_same_v<Stack<int, std::vector>::container_type, std::vector<int>>);
} // namespace TemplateAsTemplateParam


TEST_CASE("After construction", "[stack]")
{
    Stack<int> s;

    SECTION("is empty")
    {
        REQUIRE(s.empty());
    }

    SECTION("size is zero")
    {
        REQUIRE(s.size() == 0);
    }
}

TEST_CASE("Pushing an item", "[stack,push]")
{
    Stack<int> s;

    SECTION("is no longer empty")
    {
        s.push(1);

        REQUIRE(!s.empty());
    }

    SECTION("size is increased")
    {
        auto size_before = s.size();

        s.push(1);

        REQUIRE(s.size() - size_before == 1);
    }

    SECTION("recently pushed item is on a top")
    {
        s.push(4);

        REQUIRE(s.top() == 4);
    }
}

template <typename TStack>
std::vector<typename TStack::value_type> pop_all(TStack& s)
{
    std::vector<typename TStack::value_type> values(s.size());

    for (auto& item : values)
        s.pop(item);

    return values;
}

TEST_CASE("Popping an item", "[stack,pop]")
{
    Stack<int> s;

    s.push(1);
    s.push(4);

    int item;

    SECTION(" an item from a top to an argument passed by ref")
    {
        s.pop(item);

        REQUIRE(item == 4);
    }

    SECTION("size is decreased")
    {
        size_t size_before = s.size();

        s.pop(item);

        REQUIRE(size_before - s.size() == 1);
    }

    SECTION("LIFO order")
    {
        int a, b;

        s.pop(a);
        s.pop(b);

        REQUIRE(a == 4);
        REQUIRE(b == 1);
    }
}

TEST_CASE("Copy semantics", "[stack,copy]")
{
    Stack<std::string> s;
    s.push("txt1");
    s.push("txt2");
    s.push("txt3");

    SECTION("copy constructor", "[stack,copy]")
    {
        Stack<std::string> copy_s = s;

        REQUIRE_THAT(pop_all(copy_s), Equals(std::vector<std::string>{"txt3", "txt2", "txt1"}));
    }

    SECTION("copy assignment", "[stack,copy]")
    {
        SECTION("when the same capacity")
        {
            Stack<std::string> target;
            target.push("x");
            REQUIRE(target.size() == 1);

            target = s;
            REQUIRE(target.size() == 3);

            REQUIRE_THAT(pop_all(target), Equals(std::vector<std::string>{"txt3", "txt2", "txt1"}));
        }
    }
}

TEST_CASE("Copy & conversion", "[stack,copy,conversion]")
{
    Stack<int> s_ints;
    s_ints.push(1);
    s_ints.push(2);
    s_ints.push(3);

    Stack<double> s_doubles = s_ints;

    REQUIRE_THAT(pop_all(s_doubles), Equals(std::vector<double>{3.0, 2.0, 1.0}));
}

TEST_CASE("Copy & different containers", "[stack,copy,conversion]")
{
    TemplateAsTemplateParam::Stack<int, std::deque> s_ints;

    s_ints.push(1);
    s_ints.push(2);
    s_ints.push(3);

    TemplateAsTemplateParam::Stack<int, std::vector> s_ints_with_vec = s_ints;

    REQUIRE(pop_all(s_ints_with_vec) == std::vector{3, 2, 1});
}

TEST_CASE("Copy & different containers with convertible types", "[stack,copy,conversion]")
{
    TemplateAsTemplateParam::Stack<int, std::deque> s_ints;
    s_ints.push(1);
    s_ints.push(2);
    s_ints.push(3);

    TemplateAsTemplateParam::Stack<double, std::vector> s_doubles_with_vec = s_ints;

    REQUIRE(pop_all(s_doubles_with_vec) == std::vector{3.0, 2.0, 1.0});
}

TEST_CASE("generic lambda - C++20")
{
    using T = std::string;

    std::vector<T> vec;

    auto pusher1 = [&vec](auto&& item) {
        vec.push_back(std::forward<decltype(item)>(item));
    };

    auto pusher2 = [&vec]<typename TItem>(TItem&& item) {
        vec.push_back(std::forward<TItem>(item));
    };

    auto pusher3 = []<typename T>(std::vector<T>& vec, T&& item) {
        vec.push_back(std::forward<T>(item));
    };
}