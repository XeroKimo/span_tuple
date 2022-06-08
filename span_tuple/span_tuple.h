#pragma once
#include <tuple>
#include <span>

namespace std
{
    template<class Ty, size_t Extent>
    class array;
}

namespace xk
{
    template<class First, size_t Extent, class... Ty>
    class span_tuple;

    template <class First, class... Others>
    struct span_tuple_iterator {
#ifdef __cpp_lib_concepts
        using iterator_concept = std::contiguous_iterator_tag;
#endif // __cpp_lib_concepts
        using iterator_category = std::random_access_iterator_tag;
        using value_type = std::tuple<std::remove_cv_t<First>, std::remove_cv_t<Others>...>;
        using difference_type = ptrdiff_t;
        using pointer = std::tuple<First*, Others*...>;
        using reference = std::tuple<First&, Others&...>;

        _NODISCARD constexpr reference operator*() const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
            _STL_VERIFY(std::get<0>(_Mybegin), "cannot dereference value-initialized span iterator");
            _STL_VERIFY(std::get<0>(_Myptr) < std::get<0>(_Myend), "cannot dereference end span iterator");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
            return std::apply([](auto&&... elements)
            {
                return reference(*elements...);
            }, _Myptr);
        }

        _NODISCARD constexpr pointer operator->() const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
            _STL_VERIFY(std::get<0>(_Mybegin), "cannot dereference value-initialized span iterator");
            _STL_VERIFY(std::get<0>(_Myptr) < std::get<0>(_Myend), "cannot dereference end span iterator");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
            return _Myptr;
        }

        constexpr span_tuple_iterator& operator++() noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
            _STL_VERIFY(std::get<0>(_Mybegin), "cannot increment value-initialized span iterator");
            _STL_VERIFY(std::get<0>(_Myptr) < std::get<0>(_Myend), "cannot increment span iterator past end");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
            std::apply([](auto*&... ptrs)
            {
                (++ptrs, ...);
            }, _Myptr);
            return *this;
        }

        constexpr span_tuple_iterator operator++(int) noexcept {
            span_tuple_iterator _Tmp{ *this };
            ++* this;
            return _Tmp;
        }

        constexpr span_tuple_iterator& operator--() noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
            _STL_VERIFY(std::get<0>(_Mybegin), "cannot decrement value-initialized span iterator");
            _STL_VERIFY(std::get<0>(_Mybegin) < std::get<0>(_Myptr), "cannot decrement span iterator before begin");
#endif // _ITERATOR_DEBUG_LEVEL >= 1

            std::apply([](auto*&... ptrs)
            {
                (--ptrs, ...);
            }, _Myptr);
            return *this;
        }

        constexpr span_tuple_iterator operator--(int) noexcept {
            span_tuple_iterator _Tmp{ *this };
            --* this;
            return _Tmp;
        }

        constexpr void _Verify_offset([[maybe_unused]] const difference_type _Off) const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
            if(_Off != 0) {
                _STL_VERIFY(std::get<0>(_Mybegin), "cannot seek value-initialized span iterator");
            }

            if(_Off < 0) {
                _STL_VERIFY(std::get<0>(_Myptr) - std::get<0>(_Mybegin) >= -_Off, "cannot seek span iterator before begin");
            }

            if(_Off > 0) {
                _STL_VERIFY(std::get<0>(_Myend) - std::get<0>(_Myptr) >= _Off, "cannot seek span iterator after end");
            }
#endif // _ITERATOR_DEBUG_LEVEL >= 1
        }

        constexpr span_tuple_iterator& operator+=(const difference_type _Off) noexcept {
            _Verify_offset(_Off);

            std::apply([](auto*&... ptrs)
            {
                ((ptrs += _Off), ...);
            }, _Myptr);
            return *this;
        }

        _NODISCARD constexpr span_tuple_iterator operator+(const difference_type _Off) const noexcept {
            span_tuple_iterator _Tmp{ *this };
            _Tmp += _Off;
            return _Tmp;
        }

        _NODISCARD friend constexpr span_tuple_iterator operator+(const difference_type _Off, span_tuple_iterator _Next) noexcept {
            _Next += _Off;
            return _Next;
        }

        constexpr span_tuple_iterator& operator-=(const difference_type _Off) noexcept {
            _Verify_offset(-_Off);

            std::apply([](auto*&... ptrs)
            {
                ((ptrs -= _Off), ...);
            }, _Myptr);
            return *this;
        }

        _NODISCARD constexpr span_tuple_iterator operator-(const difference_type _Off) const noexcept {
            span_tuple_iterator _Tmp{ *this };
            _Tmp -= _Off;
            return _Tmp;
        }

        _NODISCARD constexpr difference_type operator-(const span_tuple_iterator& _Right) const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
            _STL_VERIFY(
                _Mybegin == _Right._Mybegin && _Myend == _Right._Myend, "cannot subtract incompatible span iterators");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
            return _Myptr - _Right._Myptr;
        }

        _NODISCARD constexpr reference operator[](const difference_type _Off) const noexcept {
            return *(*this + _Off);
        }

        _NODISCARD constexpr bool operator==(const span_tuple_iterator& _Right) const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
            _STL_VERIFY(_Mybegin == _Right._Mybegin && _Myend == _Right._Myend,
                "cannot compare incompatible span iterators for equality");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
            return _Myptr == _Right._Myptr;
        }

        _NODISCARD constexpr std::strong_ordering operator<=>(const span_tuple_iterator& _Right) const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
            _STL_VERIFY(
                _Mybegin == _Right._Mybegin && _Myend == _Right._Myend, "cannot compare incompatible span iterators");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
            return _Myptr <=> _Right._Myptr;
        }

#if _ITERATOR_DEBUG_LEVEL >= 1
        friend constexpr void _Verify_range(const span_tuple_iterator& _First, const span_tuple_iterator& _Last) noexcept {
            _STL_VERIFY(_First._Mybegin == _Last._Mybegin && _First._Myend == _Last._Myend,
                "span iterators from different views do not form a range");
            _STL_VERIFY(_First._Myptr <= _Last._Myptr, "span iterator range transposed");
        }
#endif // _ITERATOR_DEBUG_LEVEL >= 1

        using _Prevent_inheriting_unwrap = span_tuple_iterator;

        _NODISCARD constexpr pointer _Unwrapped() const noexcept {
            return _Myptr;
        }

        static constexpr bool _Unwrap_when_unverified = _ITERATOR_DEBUG_LEVEL == 0;

        constexpr void _Seek_to(const pointer _It) noexcept {
            _Myptr = _It;
        }

        pointer _Myptr = nullptr;
#if _ITERATOR_DEBUG_LEVEL >= 1
        pointer _Mybegin = nullptr;
        pointer _Myend = nullptr;
#endif // _ITERATOR_DEBUG_LEVEL >= 1
    };

    template<class First, size_t Extent, class... Ty>
    struct Extent_type
    {
        using pointer = std::tuple<First*, Ty*...>;

        constexpr Extent_type() noexcept = default;

        constexpr explicit Extent_type(pointer&& data, size_t) noexcept : m_data{ std::move(data) } {}

        pointer m_data = {};
        static constexpr size_t m_size = Extent;
    };

    template<class First, class... Ty>
    struct Extent_type<First, std::dynamic_extent, Ty...>
    {
        using pointer = std::tuple<First*, Ty*...>;

        constexpr Extent_type() noexcept = default;

        constexpr explicit Extent_type(pointer&& data, const size_t size) noexcept :
            m_data{ data }, 
            m_size{ size }
        {
        }

        pointer m_data = {};
        size_t m_size{ 0 };
    };

    template<class First, size_t Extent, class... Ty>
    struct span_traits
    {
        using value_type = std::tuple<std::remove_cv_t<First>, std::remove_cv_t<Ty>...>;
        using pointer = std::tuple<First*, Ty*...>;
        using reference = std::tuple<First&, Ty&...>;
        using size_type = size_t;
    };

    template<class First, size_t Extent>
    struct span_traits<First, Extent>;

    template<class First, size_t Extent = std::dynamic_extent, class... Ty>
    class span_tuple : private Extent_type<First, Extent, Ty...>
    {
    private:
        using base = Extent_type<First, Extent, Ty...>;
        using base::m_data;
        using base::m_size;

    public:
        using value_type = typename span_traits<First, Extent, Ty...>::value_type;
        using pointer = typename span_traits<First, Extent, Ty...>::pointer;
        using reference = typename span_traits<First, Extent, Ty...>::reference;
        using size_type = typename span_traits<First, Extent, Ty...>::size_type;
        using iterator = span_tuple_iterator<First, Ty...>;
        using reverse_iterator = ::std::reverse_iterator<iterator>;

    public:
        constexpr span_tuple() noexcept requires (Extent == 0 || Extent == std::dynamic_extent) = default;

        template <std::_Span_compatible_iterator<First> It, std::_Span_compatible_iterator<Ty>... OtherIt>
        constexpr explicit(Extent != std::dynamic_extent) span_tuple(It FirstIt, size_type Count, OtherIt... otherIt) noexcept // strengthened
            : base(std::forward_as_tuple(std::to_address(std::_Get_unwrapped_n(FirstIt, Count)), std::to_address(std::_Get_unwrapped_n(otherIt, Count))...), Count) {
#if _CONTAINER_DEBUG_LEVEL > 0
            if constexpr(Extent != std::dynamic_extent) {
                _STL_VERIFY(Count == Extent,
                    "Cannot construct span with static extent from range [first, first + count) as count != extent");
            }
#endif // _CONTAINER_DEBUG_LEVEL > 0
        }

        template <std::_Span_compatible_iterator<First> _It, std::_Span_compatible_sentinel<_It> _Sentinel, std::_Span_compatible_iterator<Ty>... OtherIt>
        constexpr explicit(Extent != std::dynamic_extent) span_tuple(_It _First, _Sentinel _Last, OtherIt... otherIt)
            noexcept(noexcept(_Last - _First)) // strengthened
            : base(std::forward_as_tuple(std::to_address(_First), std::to_address(otherIt)...), static_cast<size_type>(_Last - _First)) 
        {
            std::_Adl_verify_range(_First, _Last);
#if _CONTAINER_DEBUG_LEVEL > 0
            if constexpr(Extent != std::dynamic_extent) {
                _STL_VERIFY(_Last - _First == Extent,
                    "Cannot construct span with static extent from range [first, last) as last - first != extent");
            }
#endif // _CONTAINER_DEBUG_LEVEL > 0
        }

        template<size_t Size>
            requires(Extent == std::dynamic_extent || Extent == Size)
        span_tuple(First (&first)[Size], Ty (&...elm)[Size]) :
            base(std::forward_as_tuple(first, elm...), Size)
        {

        }

        template <class OtherFirst, class... OtherTy, size_t Size>
            requires (Extent == std::dynamic_extent || Extent == Size) &&
                std::is_convertible_v<OtherFirst(*)[], First(*)[]> &&
                (std::is_convertible_v<OtherTy(*)[], Ty(*)[]> && ...)
        span_tuple(std::array<OtherFirst, Size>& first, std::array<OtherTy, Size>&... elm) :
            base(std::forward_as_tuple(first.data(), elm.data()...), std::size(first))
        {

        }

        template <class OtherFirst, class... OtherTy, size_t Size>
            requires (Extent == std::dynamic_extent || Extent == Size) &&
                std::is_convertible_v<const OtherFirst(*)[], First(*)[]> &&
                (std::is_convertible_v<const OtherTy(*)[], Ty(*)[]>, ...)
        span_tuple(const std::array<OtherFirst, Size>& first, const std::array<OtherTy, Size>&... elm) :
            base(std::forward_as_tuple(first.data(), elm.data()...), std::size(first))
        {

        }

        template <class OtherFirst, class... OtherTy, size_t OtherExtent>
            requires (Extent == std::dynamic_extent || OtherExtent == std::dynamic_extent || Extent == OtherExtent) &&
                std::is_convertible_v<OtherFirst(*)[], First(*)[]> &&
                (std::is_convertible_v<OtherTy(*)[], Ty(*)[]> && ...)
            constexpr explicit(Extent != std::dynamic_extent && OtherExtent == std::dynamic_extent)
        span_tuple(const span_tuple<OtherFirst, OtherExtent, OtherTy...>& other) noexcept
                    : base(other.data(), other.size())
            {
#if _CONTAINER_DEBUG_LEVEL > 0
            if constexpr(Extent != std::dynamic_extent) {
                _STL_VERIFY(other.size() == Extent,
                    "Cannot construct span with static extent from other span as other.size() != extent");
            }
#endif // _CONTAINER_DEBUG_LEVEL > 0
        }
        template <std::_Span_compatible_range<First> _Rng, std::_Span_compatible_range<Ty>... OtherRng>
            constexpr explicit(Extent != std::dynamic_extent) span_tuple(_Rng&& _Range, OtherRng&&... OtherRange)
                    : base(std::forward_as_tuple(::std::ranges::data(_Range), ::std::ranges::data(OtherRange)...), static_cast<size_type>(::std::ranges::size(_Range))) {
#if _CONTAINER_DEBUG_LEVEL > 0
                    if constexpr(Extent != std::dynamic_extent) {
                        _STL_VERIFY(::std::ranges::size(_Range) == Extent,
                            "Cannot construct span with static extent from range r as std::ranges::size(r) != extent");
                    }
#endif // _CONTAINER_DEBUG_LEVEL > 0
                }

    public:
        constexpr reference operator[](size_t offset) const noexcept
        {
            return std::apply([offset](auto&&... elements)
            {
                return reference(elements[offset]...);
            }, m_data);
        }

    public:
        constexpr reference front() const noexcept
        {
            return std::apply([](auto&&... elements)
            {
                return reference(*elements...);
            }, m_data);
        }

        template<size_t Index>
        constexpr std::tuple_element_t<Index, reference> front() const noexcept
        {
            return *get<Index>(m_data);
        }
        constexpr reference back() const noexcept
        {
            return std::apply([offset = m_size - 1](auto&&... elements)
            {
                return reference(elements[offset]...);
            }, m_data);
        }

        template<size_t Index>
        constexpr std::tuple_element_t<Index, reference> back() const noexcept
        {
            return get<Index>(m_data)[m_size - 1];
        }

        constexpr pointer data() const noexcept { return m_data; }

        template<size_t Index>
        constexpr std::tuple_element_t<Index, pointer> data() const noexcept 
        { 
            return get<Index>(m_data);
        }

        constexpr size_t size() const noexcept { return m_size; }
        constexpr size_t size_bytes() const noexcept { return ((sizeof(Ty) * m_size) + ...); }
        template<size_t Index>
        constexpr size_t size_bytes() const noexcept { return sizeof(std::tuple_element_t<Index, value_type>) * m_size; }
        constexpr bool empty() const noexcept { return m_size == 0; }

        template<size_t Index, class First, size_t Extent, class... Ty>
        friend constexpr auto get(span_tuple<First, Extent, Ty...> span);


    // [span.iterators] Iterator support
        _NODISCARD constexpr iterator begin() const noexcept {

            const auto _End = std::apply([size = m_size](auto*... ptrs)
            {
                ((ptrs += size), ...);
                return std::tuple(ptrs...);
        }, m_data);
#if _ITERATOR_DEBUG_LEVEL >= 1
            return { m_data, m_data, _End };
#else // ^^^ _ITERATOR_DEBUG_LEVEL >= 1 ^^^ // vvv _ITERATOR_DEBUG_LEVEL == 0 vvv
            return { m_data };
#endif // _ITERATOR_DEBUG_LEVEL
        }

        _NODISCARD constexpr iterator end() const noexcept {
            const auto _End = std::apply([size = m_size](auto*... ptrs)
            {
                ((ptrs += size), ...);
                return std::tuple(ptrs...);
            }, m_data);
#if _ITERATOR_DEBUG_LEVEL >= 1
            return { _End, m_data, _End };
#else // ^^^ _ITERATOR_DEBUG_LEVEL >= 1 ^^^ // vvv _ITERATOR_DEBUG_LEVEL == 0 vvv
            return { _End };
#endif // _ITERATOR_DEBUG_LEVEL
        }

        _NODISCARD constexpr reverse_iterator rbegin() const noexcept {
            return reverse_iterator{ end() };
        }

        _NODISCARD constexpr reverse_iterator rend() const noexcept {
            return reverse_iterator{ begin() };
        }

        _NODISCARD constexpr pointer _Unchecked_begin() const noexcept {
            return m_data;
        }

        _NODISCARD constexpr pointer _Unchecked_end() const noexcept {
            return m_data + m_size;
        }
    };


    template<class First, size_t Extent>
    class span_tuple<First, Extent>;

    template<size_t Index, class First, size_t Extent, class... Ty>
    constexpr auto get(span_tuple<First, Extent, Ty...> span)
    {
        return std::span<std::tuple_element_t<Index, typename span_tuple<First, Extent, Ty...>::value_type>, Extent>(get<Index>(span.data()), span.size());
    }
}