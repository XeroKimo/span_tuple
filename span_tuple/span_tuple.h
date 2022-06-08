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

    template<class... Ty>
    class span_tuple_iterator
    {

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
        constexpr bool empty() const noexcept { return m_size == 0; }

        template<size_t Index, class First, size_t Extent, class... Ty>
        friend constexpr auto get(span_tuple<First, Extent, Ty...> span);
    };


    template<class First, size_t Extent>
    class span_tuple<First, Extent>;

    template<size_t Index, class First, size_t Extent, class... Ty>
    constexpr auto get(span_tuple<First, Extent, Ty...> span)
    {
        return std::span<std::tuple_element_t<Index, typename span_tuple<First, Extent, Ty...>::value_type>, Extent>(get<Index>(span.data()), span.size());
    }
}