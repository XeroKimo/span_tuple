#pragma once
#include <array>
#include <tuple>
#include <type_traits>

#if __has_cpp_attribute(nodiscard)
#define XK_ARRAY_TUPLE_NODISCARD [[nodiscard]]
#else
#define XK_ARRAY_TUPLE_NODISCARD
#endif

namespace xk
{
    template <class... Ty>
    struct array_tuple_iterator {
#ifdef __cpp_lib_concepts
        using iterator_concept = std::contiguous_iterator_tag;
#endif // __cpp_lib_concepts
        using iterator_category = std::random_access_iterator_tag;
        using value_type = std::tuple<std::remove_cv_t<Ty>...>;
        using difference_type = ptrdiff_t;
        using pointer = std::tuple<Ty*...>;
        using reference = std::tuple<Ty&...>;

        XK_ARRAY_TUPLE_NODISCARD constexpr reference operator*() const noexcept {
            return std::apply([](auto&&... elements)
            {
                return reference(*elements...);
            }, _Myptr);
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr pointer operator->() const noexcept {
            return _Myptr;
        }

        constexpr array_tuple_iterator& operator++() noexcept {
            std::apply([](auto*&... ptrs)
            {
                (++ptrs, ...);
            }, _Myptr);
            return *this;
        }

        constexpr array_tuple_iterator operator++(int) noexcept {
            array_tuple_iterator _Tmp{ *this };
            ++* this;
            return _Tmp;
        }

        constexpr array_tuple_iterator& operator--() noexcept {
            std::apply([](auto*&... ptrs)
            {
                (--ptrs, ...);
            }, _Myptr);
            return *this;
        }

        constexpr array_tuple_iterator operator--(int) noexcept {
            array_tuple_iterator _Tmp{ *this };
            --* this;
            return _Tmp;
        }

        constexpr array_tuple_iterator& operator+=(const difference_type _Off) noexcept {
            std::apply([=](auto*&... ptrs)
            {
                ((ptrs += _Off), ...);
            }, _Myptr);
            return *this;
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr array_tuple_iterator operator+(const difference_type _Off) const noexcept {
            array_tuple_iterator _Tmp{ *this };
            _Tmp += _Off;
            return _Tmp;
        }

        XK_ARRAY_TUPLE_NODISCARD friend constexpr array_tuple_iterator operator+(const difference_type _Off, array_tuple_iterator _Next) noexcept {
            _Next += _Off;
            return _Next;
        }

        constexpr array_tuple_iterator& operator-=(const difference_type _Off) noexcept {
            std::apply([=](auto*&... ptrs)
            {
                ((ptrs -= _Off), ...);
            }, _Myptr);
            return *this;
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr array_tuple_iterator operator-(const difference_type _Off) const noexcept {
            array_tuple_iterator _Tmp{ *this };
            _Tmp -= _Off;
            return _Tmp;
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr difference_type operator-(const array_tuple_iterator& _Right) const noexcept {
            return _Myptr - _Right._Myptr;
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr reference operator[](const difference_type _Off) const noexcept {
            return *(*this + _Off);
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr bool operator==(const array_tuple_iterator& _Right) const noexcept {

            return _Myptr == _Right._Myptr;
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr std::strong_ordering operator<=>(const array_tuple_iterator& _Right) const noexcept {
            return _Myptr <=> _Right._Myptr;
        }

        pointer _Myptr = nullptr;
    };

    template <size_t Size, class... Ty>
    class array_tuple { // fixed size array of values
    public:
        using value_type = std::tuple<Ty[Size]...>;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = std::tuple<Ty*...>;
        using const_pointer = std::tuple<const Ty*...>;
        using reference = std::tuple<Ty&...>;
        using const_reference = std::tuple<const Ty...>;

        using iterator = array_tuple_iterator<Ty...>;
        using const_iterator = array_tuple_iterator<const Ty...>;

        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        value_type m_arrays;

    public:
        constexpr void fill(const std::tuple<Ty...>& _Value) 
        {
            std::fill_n(m_arrays, Size, _Value);
        }

        constexpr void swap(array_tuple& _Other) noexcept((std::_Is_nothrow_swappable<Ty>::value, ...))
        {
            std::swap(m_arrays, _Other.m_arrays);
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr iterator begin() noexcept {
            return iterator(std::apply([](auto*... ptrs) { return pointer(&ptrs[0]...); }, m_arrays));
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_iterator begin() const noexcept {
            return const_iterator(std::apply([](auto*... ptrs) { return const_pointer(&ptrs[0]...); }, m_arrays));
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr iterator end() noexcept {
            return iterator(std::apply([](auto*... ptrs) { return pointer(&ptrs[Size]...); }, m_arrays));
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_iterator end() const noexcept {
            return const_iterator(std::apply([](auto*... ptrs) { return const_pointer(&ptrs[Size]...); }, m_arrays));
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr reverse_iterator rbegin() noexcept {
            return reverse_iterator(end());
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr reverse_iterator rend() noexcept {
            return reverse_iterator(begin());
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_iterator cbegin() const noexcept {
            return begin();
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_iterator cend() const noexcept {
            return end();
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_reverse_iterator crbegin() const noexcept {
            return rbegin();
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_reverse_iterator crend() const noexcept {
            return rend();
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr size_type size() const noexcept 
        {
            return Size;
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr size_type max_size() const noexcept 
        {
            return Size;
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr bool empty() const noexcept 
        {
            return false;
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr reference at(size_type _Pos)
        {
            if(Size <= _Pos) 
            {
                //_Xran();
            }

            return m_arrays[_Pos];
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_reference at(size_type _Pos) const 
        {
            if(Size <= _Pos) 
            {

                //_Xran();
            }

            return m_arrays[_Pos];
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr reference operator[](_In_range_(0, Size - 1) size_type _Pos) noexcept /* strengthened */
        {
            return std::apply([_Pos](auto*... ptrs)
            {
                return reference(ptrs[_Pos]...);
            }, m_arrays);
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_reference operator[](_In_range_(0, Size - 1) size_type _Pos) const noexcept
        {
            return std::apply([_Pos](auto*... ptrs)
            {
                return const_reference(ptrs[_Pos]...);
            }, m_arrays);
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr reference front() noexcept /* strengthened */ {
            return std::apply([](auto*... ptrs)
            {
                return reference(ptrs[0]...);
            }, m_arrays);
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_reference front() const noexcept /* strengthened */ {
            return std::apply([](auto*... ptrs)
            {
                return const_reference(ptrs[0]...);
            }, m_arrays);
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr reference back() noexcept /* strengthened */ {
            return std::apply([](auto*... ptrs)
            {
                return reference(ptrs[Size - 1]...);
            }, m_arrays);
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_reference back() const noexcept /* strengthened */ {

            return std::apply([](auto*... ptrs)
            {
                return const_reference(ptrs[Size - 1]...);
            }, m_arrays);
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr pointer data() noexcept {
            return m_arrays;
        }

        XK_ARRAY_TUPLE_NODISCARD constexpr const_pointer data() const noexcept {
            return m_arrays;
        }
    };
}