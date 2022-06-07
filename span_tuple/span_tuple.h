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
    template<size_t Extent, class... Ty>
    class span_tuple;

    template<class... Ty>
    class span_tuple_iterator
    {

    };

    template<size_t Extent, class... Ty>
    struct Extent_type
    {
        using pointer = std::tuple<Ty*...>;

        constexpr Extent_type() noexcept = default;

        constexpr explicit Extent_type(pointer&& data, size_t) noexcept : m_data{ std::move(data) } {}

        pointer m_data = {};
        static constexpr size_t m_size = Extent;
    };

    template <class... Ty>
    struct Extent_type<std::dynamic_extent, Ty...> 
    {
        using pointer = std::tuple<Ty*...>;

        constexpr Extent_type() noexcept = default;

        constexpr explicit Extent_type(pointer&& data, const size_t size) noexcept :
            m_data{ data }, 
            m_size{ size }
        {
        }

        pointer m_data = {};
        size_t m_size{ 0 };

        template<size_t... Index, size_t Extent, class... Ty>
        friend constexpr auto get(span_tuple<Extent, Ty...> span);
    };

    template<size_t Extent, class... Ty>
    struct span_traits
    {
        using value_type = std::tuple<Ty...>;
        using pointer = std::tuple<Ty*...>;
        using reference = std::tuple<Ty&...>;
        using size_type = size_t;
    };

    template<size_t Extent, class Ty>
    struct span_traits<Extent, Ty>
    {
        using value_type = Ty;
        using pointer = Ty*;
        using reference = Ty&;
        using size_type = size_t;
    };


    template<size_t Extent, class... Ty>
    class span_tuple : private Extent_type<Extent, Ty...>
    {
    private:
        using base = Extent_type<Extent, Ty...>;
        using base::m_data;
        using base::m_size;

    public:
        using value_type = typename span_traits<Extent, Ty...>::value_type;
        using pointer = typename span_traits<Extent, Ty...>::pointer;
        using reference = typename span_traits<Extent, Ty...>::reference;
        using size_type = typename span_traits<Extent, Ty...>::size_type;

    private:
        using base::base;

    public:
        constexpr span_tuple() noexcept requires (Extent == 0 || Extent == std::dynamic_extent) = default;

        template<size_t Size>
            requires(Extent == std::dynamic_extent || Extent == Size)
        span_tuple(Ty (&...elm)[Size]) :
            base(std::forward_as_tuple(elm...), Size)
        {

        }

        template <class... OtherTy, size_t Size>
            requires (Extent == std::dynamic_extent || Extent == Size) &&
                (std::is_convertible_v<OtherTy(*)[], Ty(*)[]> && ...)
        span_tuple(std::array<OtherTy, Size>&... elm) :
            base(std::forward_as_tuple(elm.data()...), std::size(std::get<0>(std::forward_as_tuple(elm...))))
        {

        }

        template <class... OtherTy, size_t Size>
            requires (Extent == std::dynamic_extent || Extent == Size) &&
                (std::is_convertible_v<const OtherTy(*)[], Ty(*)[]>, ...)
        span_tuple(const std::array<OtherTy, Size>&... elm) :
            base(std::forward_as_tuple(elm.data()...), std::size(std::get<0>(std::forward_as_tuple(elm...))))
        {

        }

        template <class... OtherTy, size_t OtherExtent>
            requires (Extent == std::dynamic_extent || OtherExtent == std::dynamic_extent || Extent == OtherExtent) &&
                (std::is_convertible_v<const OtherTy(*)[], Ty(*)[]>, ...)
            constexpr explicit(Extent != std::dynamic_extent && OtherExtent == std::dynamic_extent)
        span_tuple(const span_tuple<OtherExtent, OtherTy...>& other) noexcept
                    : base(other.data(), other.size())
            {
#if _CONTAINER_DEBUG_LEVEL > 0
            if constexpr(Extent != std::dynamic_extent) {
                _STL_VERIFY(other.size() == Extent,
                    "Cannot construct span with static extent from other span as other.size() != extent");
            }
#endif // _CONTAINER_DEBUG_LEVEL > 0
        }
            // clang-format on

    public:
        constexpr pointer data() const noexcept { return m_data; }

        template<size_t... Index>
        constexpr auto data() const noexcept 
        { 
            if constexpr(sizeof...(Index) == 1)
                return (std::get<Index>(m_data), ...);
            else
                return std::tuple(std::get<Index>(m_data)...);
        }

        constexpr size_t size() const noexcept { return m_size; }
        constexpr size_t size_bytes() const noexcept { return ((sizeof(Ty) * m_size) + ...); }
        constexpr bool empty() const noexcept { return m_size == 0; }

        template<size_t... Index, size_t Extent, class... Ty>
        friend constexpr auto get(span_tuple<Extent, Ty...> span);
    };


    template<size_t Extent, class Ty>
    class span_tuple<Extent, Ty> : public std::span<Ty, Extent>
    {
    private:
        using base = std::span<Ty, Extent>;

    public:
        using base::base;

    public:
        template<size_t... Index, size_t Extent, class... Ty>
        friend constexpr auto get(span_tuple<Extent, Ty...> span);
    };

    template<size_t... Index, size_t Extent, class... Ty>
    constexpr auto get(span_tuple<Extent, Ty...> span)
    {
        if constexpr(sizeof...(Index) == 1)
            return span_tuple<Extent, std::tuple_element_t<Index, typename span_tuple<Extent, Ty...>::value_type>...>(std::get<Index>(span.data())..., span.size());
        else
            return span_tuple<Extent, std::tuple_element_t<Index, typename span_tuple<Extent, Ty...>::value_type>...>(std::forward_as_tuple(span.data<Index>()...), span.size());
    }
}