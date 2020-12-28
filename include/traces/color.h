#ifndef TRACES_COLOR_H_INCLUDED
#define TRACES_COLOR_H_INCLUDED

#include <cstdint>
#include <tuple>

#include "functionname.h"
#include "modules.h"
#include "config.h"

namespace traces
{

struct Color
{
    std::uint8_t r, g, b;

    constexpr operator std::uint32_t() const
    {
        return (r << 16) | (g << 8) | b;
    }

    constexpr Color& operator+=(const Color& other)
    {
        r += other.r;
        g += other.g;
        b += other.b;

        return *this;
    }

    constexpr bool operator==(const Color& other) const
    {
        return std::tie(r, g, b) == std::tie(other.r, other.g, other.b);
    }

    constexpr bool operator!=(const Color& other) const
    {
        return !(*this == other);
    }

    static constexpr Color rgb(const int r, const int g, const int b)
    {
        return rgb(
            static_cast<std::uint8_t>(r),
            static_cast<std::uint8_t>(g),
            static_cast<std::uint8_t>(b));
    }

    static constexpr Color
        rgb(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b)
    {
        return Color{.r = r, .g = g, .b = b};
    }

    static constexpr Color black()
    {
        return rgb(0, 0, 0);
    }

    static constexpr Color white()
    {
        return rgb(255, 255, 255);
    }
};

namespace colors
{

// From https://materialuicolors.co/
// (Level: 500)
namespace material
{

constexpr auto Red        = Color::rgb(244, 67, 54);
constexpr auto Pink       = Color::rgb(233, 30, 99);
constexpr auto Purple     = Color::rgb(156, 39, 176);
constexpr auto DeepPurple = Color::rgb(103, 58, 183);
constexpr auto Indigo     = Color::rgb(63, 81, 181);
constexpr auto Blue       = Color::rgb(33, 150, 243);
constexpr auto LightBlue  = Color::rgb(3, 169, 244);
constexpr auto Cyan       = Color::rgb(0, 188, 212);
constexpr auto Teal       = Color::rgb(0, 150, 136);
constexpr auto Green      = Color::rgb(76, 175, 80);
constexpr auto LightGreen = Color::rgb(139, 195, 74);
constexpr auto Lime       = Color::rgb(205, 220, 57);
constexpr auto Yellow     = Color::rgb(255, 235, 59);
constexpr auto Amber      = Color::rgb(255, 193, 7);
constexpr auto Orange     = Color::rgb(255, 152, 0);
constexpr auto DeepOrange = Color::rgb(255, 87, 34);
constexpr auto Brown      = Color::rgb(121, 85, 72);
constexpr auto Grey       = Color::rgb(158, 158, 158);
constexpr auto BlueGrey   = Color::rgb(96, 125, 139);

constexpr auto all()
{
    return std::array{
        LightGreen,
        Orange,
        LightBlue,
        Green,
        DeepPurple,
        Lime,
        Indigo,
        DeepOrange,
        Teal,
        Amber,
        Blue,
        Purple,
        Yellow,
        Red,
        Brown,
        Grey,
        BlueGrey,
    };
};

} // namespace material

} // namespace colors

TRACES_CONSTEXPR_FUNCTION Color moduleColor(std::string_view name)
{
    constexpr auto palette = ::traces::colors::material::all();

    if(const auto index = moduleIndex(name))
    {
        return palette[*index % palette.size()];
    }
    else
    {
        // We return black by default to tell tracy gui to use its own
        // color criteria
        return Color::black();
    }
}

TRACES_CONSTEXPR_FUNCTION Color
    functionColor(const ::traces::detail::FunctionName& functionName)
{
    return moduleColor(functionName.fullName());
}

TRACES_CONSTEXPR_FUNCTION Color messageColor(std::string_view module)
{
    const Color color = moduleColor(module);

    if(color == Color::black())
    {
        return Color::white();
    }
    else
    {
        return color;
    }
}
} // namespace traces

#endif // TRACES_COLOR_H_INCLUDED
