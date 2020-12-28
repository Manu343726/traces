#ifndef TRACES_FUNCTIONAME_H_INCLUDED
#define TRACES_FUNCTIONAME_H_INCLUDED

#include <string_view>
#include "config.h"

namespace traces
{

namespace detail
{

struct FunctionName
{
    TRACES_CONSTEXPR_FUNCTION FunctionName(std::string_view prettyFunctionString);

    TRACES_CONSTEXPR_FUNCTION std::string_view displayName() const;
    TRACES_CONSTEXPR_FUNCTION std::string_view name() const;
    TRACES_CONSTEXPR_FUNCTION std::string_view fullDisplayName() const;
    TRACES_CONSTEXPR_FUNCTION std::string_view fullName() const;

private:
    struct ParsedName
    {
        std::string_view withoutArgs;
        std::string_view withoutResultType;
        std::string_view wihtoutResultTypeAndArgs;
    };

    ParsedName _parsedName;

    static TRACES_CONSTEXPR_FUNCTION ParsedName parse(std::string_view prettyFunctionString);
    static TRACES_CONSTEXPR_FUNCTION std::string_view withoutQualifier(std::string_view name);
};


TRACES_CONSTEXPR_FUNCTION FunctionName::FunctionName(std::string_view prettyFunctionString) :
    _parsedName{parse(prettyFunctionString)}
{
}

TRACES_CONSTEXPR_FUNCTION FunctionName::ParsedName FunctionName::parse(std::string_view prettyFunctionString)
{
    ParsedName result;

    const auto openParenIndex = prettyFunctionString.find_last_of('(');

    result.withoutArgs = prettyFunctionString.substr(0, openParenIndex);

    const auto spaceBetweenResultTypeAndName = result.withoutArgs.rfind(' ');

    result.withoutResultType = prettyFunctionString.substr(spaceBetweenResultTypeAndName + 1);
    result.wihtoutResultTypeAndArgs = result.withoutArgs.substr(spaceBetweenResultTypeAndName + 1);

    return result;
}

TRACES_CONSTEXPR_FUNCTION std::string_view FunctionName::withoutQualifier(std::string_view name)
{
    const auto lastQualifierIndex = name.find_last_of("::");

    if (lastQualifierIndex == std::string_view::npos)
    {
        return name;
    }
    else
    {
        return name.substr(lastQualifierIndex);
    }
}

TRACES_CONSTEXPR_FUNCTION std::string_view FunctionName::fullName() const
{
    return _parsedName.wihtoutResultTypeAndArgs;
}

TRACES_CONSTEXPR_FUNCTION std::string_view FunctionName::fullDisplayName() const
{
    return _parsedName.withoutResultType;
}

TRACES_CONSTEXPR_FUNCTION std::string_view FunctionName::displayName() const
{
    return withoutQualifier(fullDisplayName());
}

TRACES_CONSTEXPR_FUNCTION std::string_view FunctionName::name() const
{
    return withoutQualifier(fullName());
}

} // namespace detail

} // namespace traces

#endif // TRACES_FUNCTIONAME_H_INCLUDED
