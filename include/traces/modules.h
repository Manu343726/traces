#ifndef TRACES_MODULES_H_INCLUDED
#define TRACES_MODULES_H_INCLUDED

#include <array>
#include <optional>
#include <string_view>
#include <vector>

#include "color.h"
#include "config.h"

#ifndef TRACES_MODULES
#define TRACES_MODULES "''"
#endif

namespace traces
{

namespace detail
{

TRACES_CONSTEXPR_FUNCTION std::size_t total_modules(std::string_view modules)
{
    std::size_t result = 0;

    for(const char c : modules)
    {
        result += (c == '\'');
    }

    if(result % 2 != 0)
    {
        throw "Unmatched quote";
    }

    return result / 2;
}

template<std::size_t Value>
using size_constant = std::integral_constant<std::size_t, Value>;

template<std::size_t TotalModules>
TRACES_CONSTEXPR_FUNCTION auto
    parse_modules(std::string_view modules, size_constant<TotalModules>)
{
    bool        expecting_start_quote = true;
    std::size_t last_quote_index      = 0;
    std::size_t total_modules_parsed  = 0;

    std::array<std::string_view, TotalModules> result;

    for(std::size_t i = 0; i < modules.size(); ++i)
    {
        const char c = modules[i];

        if(c == '\'')
        {
            if(not expecting_start_quote)
            {
                result[total_modules_parsed++] = modules.substr(
                    last_quote_index + 1, i - last_quote_index - 1);

                if(total_modules_parsed > TotalModules)
                {
                    throw "Parse error";
                }
            }

            last_quote_index      = i;
            expecting_start_quote = not expecting_start_quote;
        }
    }

    return result;
}

inline std::vector<std::string_view> parse_modules(std::string_view modules)
{
    bool        expecting_start_quote = true;
    std::size_t last_quote_index      = 0;
    std::size_t total_modules_parsed  = 0;

    std::vector<std::string_view> result;

    for(std::size_t i = 0; i < modules.size(); ++i)
    {
        const char c = modules[i];

        if(c == '\'')
        {
            if(not expecting_start_quote)
            {
                result.push_back("");
                result[total_modules_parsed++] = modules.substr(
                    last_quote_index + 1, i - last_quote_index - 1);
            }

            last_quote_index      = i;
            expecting_start_quote = not expecting_start_quote;
        }
    }

    return result;
}

} // namespace detail

#ifdef TRACES_HAS_CONSTEXPR_STRING_VIEW
constexpr decltype(auto) modules()
{
    constexpr modules = ::traces::detail::parse_modules(
    TRACES_MODULES,
    ::traces::detail::size_constant<::traces::detail::total_modules(
        TRACES_MODULES)>{});

    return modules;
}
#else
inline decltype(auto) modules()
{
    static const auto result = ::traces::detail::parse_modules(TRACES_MODULES);
    return result;
}
#endif // TRACES_HAS_CONSTEXPR_STRING_VIEW

TRACES_CONSTEXPR_FUNCTION std::optional<std::size_t> moduleIndex(std::string_view name)
{
    std::optional<std::size_t> matchingModuleIndex;
    std::size_t                longestModuleLength = 0;

    for(std::size_t i = 0; i < ::traces::modules().size(); ++i)
    {
        const auto registeredModule = ::traces::modules()[i];

        const auto subModuleIndex = name.find(registeredModule);

        if(subModuleIndex != std::string_view::npos &&
           registeredModule.length() > longestModuleLength)
        {
            longestModuleLength = registeredModule.length();
            matchingModuleIndex = i;
        }
    }

    return matchingModuleIndex;
}

TRACES_CONSTEXPR_FUNCTION std::optional<std::string_view> module(std::string_view name)
{
    if(const auto index = moduleIndex(name))
    {
        return ::traces::modules()[*index];
    }
    else
    {
        return std::nullopt;
    }
}

} // namespace traces

#endif // TRACES_MODULES_H_INCLUDED
