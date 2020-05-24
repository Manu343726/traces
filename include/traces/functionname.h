#ifndef TRACES_FUNCTIONAME_H_INCLUDED
#define TRACES_FUNCTIONAME_H_INCLUDED

#include <string_view>

namespace traces
{

namespace detail
{

struct FunctionName
{
    constexpr FunctionName(std::string_view prettyFunctionString);

    constexpr std::string_view displayName() const;
    constexpr std::string_view name() const;
    constexpr std::string_view fullDisplayName() const;
    constexpr std::string_view fullName() const;

private:
    struct ParsedName
    {
        std::string_view withoutArgs;
        std::string_view withoutResultType;
        std::string_view wihtoutResultTypeAndArgs;
    };

    ParsedName _parsedName;

    static constexpr ParsedName parse(std::string_view prettyFunctionString);
    static constexpr std::string_view withoutQualifier(std::string_view name);
};


constexpr FunctionName::FunctionName(std::string_view prettyFunctionString) :
    _parsedName{parse(prettyFunctionString)}
{
}

constexpr FunctionName::ParsedName FunctionName::parse(std::string_view prettyFunctionString)
{
    ParsedName result;

    const auto openParenIndex = prettyFunctionString.find_last_of('(');

    result.withoutArgs = prettyFunctionString.substr(0, openParenIndex);

    const auto spaceBetweenResultTypeAndName = result.withoutArgs.rfind(' ');

    result.withoutResultType = prettyFunctionString.substr(spaceBetweenResultTypeAndName + 1);
    result.wihtoutResultTypeAndArgs = result.withoutArgs.substr(spaceBetweenResultTypeAndName + 1);

    return result;
}

constexpr std::string_view FunctionName::withoutQualifier(std::string_view name)
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

constexpr std::string_view FunctionName::fullName() const
{
    return _parsedName.wihtoutResultTypeAndArgs;
}

constexpr std::string_view FunctionName::fullDisplayName() const
{
    return _parsedName.withoutResultType;
}

constexpr std::string_view FunctionName::displayName() const
{
    return withoutQualifier(fullDisplayName());
}

constexpr std::string_view FunctionName::name() const
{
    return withoutQualifier(fullName());
}

} // namespace detail

} // namespace traces

#endif // TRACES_FUNCTIONAME_H_INCLUDED
