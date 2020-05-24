#ifndef TRACES_TOSTRING_H_INCLUDED
#define TRACES_TOSTRING_H_INCLUDED

#include <type_traits>
#include <ostream>
#include <string>
#include <optional>
#include <utility>
#include <vector>

#include "config.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace traces
{

namespace impl
{

namespace
{

template<bool Value>
using bool_constant = std::integral_constant<bool, Value>;

}

namespace traits
{

#ifdef TRACES_HAS_QSTRING

template<typename T>
using IsConvertibleToQString = std::is_convertible<T, QString>;

template<typename T, typename = void>
struct HasToStringReturningQString : std::false_type {};

template<typename T>
struct HasToStringReturningQString<T, std::void_t<decltype(std::declval<T>().toString())>>
    : std::is_same<typename std::decay<decltype(std::declval<T>().toString())>::type, QString>
{};

#else

template<typename T>
using IsConvertibleToQString = bool_constant<sizeof(T) != sizeof(T)>; // false

template<typename T>
struct HasToStringReturningQString : std::false_type {};

#endif // TRACES_HAS_QSTRING

template<typename T>
using IsConvertibleToStdString = std::is_convertible<T, std::string>;

template<typename T, typename = void>
struct HasToStringReturningStdString : std::false_type {};

template<typename T>
struct HasToStringReturningStdString<T, std::void_t<decltype(std::declval<T>().toString())>>
    : std::is_same<typename std::decay<decltype(std::declval<T>().toString())>::type, std::string>
{};

template<typename T, typename = void>
struct HasToStdStringReturningStdString : std::false_type {};

template<typename T>
struct HasToStdStringReturningStdString<T, std::void_t<decltype(std::declval<T>().toStdString())>>
    : std::is_same<typename std::decay<decltype(std::declval<T>().toStdString())>::type, std::string>
{};

template<typename T, typename = void>
struct HasDebugStringReturningStdString : std::false_type {};

template<typename T>
struct HasDebugStringReturningStdString<T, std::void_t<decltype(std::declval<T>().DebugString())>>
    : std::is_same<typename std::decay<decltype(std::declval<T>().DebugString())>::type, std::string>
{};
}

using CanBeDirectlyFormatted = std::true_type;
using CannotBeDirectlyFormatted = std::false_type;

using IsConvertibleToQString = std::true_type;
using IsNotConvertibleToQString = std::false_type;

using IsConvertibleToStdString = std::true_type;
using IsNotConvertibleToStdString = std::false_type;

using HasToStringReturningQString = std::true_type;
using HasNoToStringReturningQString = std::false_type;

using HasToStringReturningStdString = std::true_type;
using HasNoToStringReturningStdString = std::false_type;

using HasOstreamOperatorOverload = std::true_type;
using HasNoOstreamOperatorOverload = std::false_type;

using HasToStdStringReturningStdString = std::true_type;
using HasNoToStdStringReturningStdString = std::false_type;

using HasDebugStringReturningStdString = std::true_type;
using HasNoDebugStringReturningStdString = std::false_type;

template<
    typename T,
    typename DontCareIfIsConvertibleToQString,
    typename DontCareIfHasToStringReturningQString,
    typename DontCareIfHasToStringReturningStdString,
    typename DontCareIfHasToStdStringReturningStdString,
    typename DontCareIfHasDebugStringReturningStdString
>
std::string toString(
    T&& value,
    DontCareIfIsConvertibleToQString,
    IsConvertibleToStdString,
    DontCareIfHasToStringReturningQString,
    DontCareIfHasToStringReturningStdString,
    DontCareIfHasToStdStringReturningStdString,
    DontCareIfHasDebugStringReturningStdString)
{
    return static_cast<std::string>(std::forward<T>(value));
}

template<
    typename T,
    typename DontCareIfIsConvertibleToQString,
    typename DontCareIfHasToStringReturningQString,
    typename DontCareIfHasToStringReturningStdString,
    typename DontCareIfHasDebugStringReturningStdString
>
std::string toString(
    T&& value,
    DontCareIfIsConvertibleToQString,
    IsNotConvertibleToStdString,
    DontCareIfHasToStringReturningQString,
    DontCareIfHasToStringReturningStdString,
    HasToStdStringReturningStdString,
    DontCareIfHasDebugStringReturningStdString)
{
    return std::forward<T>(value).toStdString();
}

template<
    typename T,
    typename DontCareIfIsConvertibleToQString,
    typename DontCareIfHasToStringReturningQString,
    typename DontCareIfHasToStringReturningStdString
>
std::string toString(
    T&& value,
    DontCareIfIsConvertibleToQString,
    IsNotConvertibleToStdString,
    DontCareIfHasToStringReturningQString,
    DontCareIfHasToStringReturningStdString,
    HasNoToStdStringReturningStdString,
    HasDebugStringReturningStdString)
{
    return (std::forward<T>(value)).DebugString();
}

#ifdef TRACES_HAS_QSTRING
template<
    typename T,
    typename DontCareIfHasToStringReturningQString,
    typename DontCareIfHasToStringReturningStdString
>
std::string toString(
    T&& value,
    IsConvertibleToQString,
    IsNotConvertibleToStdString,
    DontCareIfHasToStringReturningQString,
    DontCareIfHasToStringReturningStdString,
    HasNoToStdStringReturningStdString,
    HasNoDebugStringReturningStdString)
{
    return static_cast<QString>(std::forward<T>(value)).toStdString();
}
#endif // TRACES_HAS_QSTRING

template<
    typename T,
    typename DontCareIfHasToStringReturningQString,
    typename DontCareIfHasToStdStringReturningStdString
>
std::string toString(
    T&& value,
    IsNotConvertibleToQString,
    IsNotConvertibleToStdString,
    DontCareIfHasToStringReturningQString,
    HasToStringReturningStdString,
    DontCareIfHasToStdStringReturningStdString,
    HasNoDebugStringReturningStdString)
{
    return std::forward<T>(value).toString();
}

template<typename T>
std::string toString(
    T&& value,
    IsNotConvertibleToQString,
    IsNotConvertibleToStdString,
    HasToStringReturningQString,
    HasNoToStringReturningStdString,
    HasNoToStdStringReturningStdString,
    HasNoDebugStringReturningStdString)
{
    return std::forward<T>(value).toString().toStdString();
}

template<typename T>
std::string toString(
    T&& value,
    IsNotConvertibleToQString,
    IsNotConvertibleToStdString,
    HasNoToStringReturningQString,
    HasNoToStringReturningStdString,
    HasNoToStdStringReturningStdString,
    HasNoDebugStringReturningStdString)
{
    return fmt::format("{}", std::forward<T>(value));
}

}

namespace
{

template<typename T>
using bool_constant = std::integral_constant<bool, T::value>;

}

template<typename T>
std::string toString(T&& value)
{
    return ::traces::impl::toString(std::forward<T>(value),
        bool_constant<::traces::impl::traits::IsConvertibleToQString<T>>(),
        bool_constant<::traces::impl::traits::IsConvertibleToStdString<T>>(),
        bool_constant<::traces::impl::traits::HasToStringReturningQString<T>>(),
        bool_constant<::traces::impl::traits::HasToStringReturningStdString<T>>(),
        bool_constant<::traces::impl::traits::HasToStdStringReturningStdString<T>>(),
        bool_constant<::traces::impl::traits::HasDebugStringReturningStdString<T>>());
}

template<typename T>
std::string toString(std::optional<T> &&optional)
{
    if (optional)
    {
        return toString(*std::move(optional));
    }
    else
    {
        return "std::nullopt";
    }
}

template<typename T>
std::string toString(const std::optional<T> &optional)
{
    if (optional)
    {
        return toString(*optional);
    }
    else
    {
        return "std::nullopt";
    }
}

template<typename T>
std::string toString(std::optional<T> &optional)
{
    return toString(std::as_const(optional));
}

template<typename String, typename FirstArg, typename... Args>
std::string toString(String&& stringBody, FirstArg&& firstArg, Args&&... args)
{
    using traces::toString;

    return fmt::format(
        toString(std::forward<String>(stringBody)),
        toString(std::forward<FirstArg>(firstArg)),
        toString(std::forward<Args>(args))...);
}

inline std::string toString()
{
    return "";
}

#ifdef TRACES_HAS_QSTRING
template<typename... Args>
QString toQString(Args&&... args)
{
    using traces::toString;

    return QString::fromStdString(toString(std::forward<Args>(args)...));
}
#endif // TRACES_HAS_QSTRING

template<typename Container, typename String>
std::string join(Container&& container, String&& separator)
{
    std::vector<std::string> items;

    for(const auto& item : std::forward<Container>(container))
    {
        items.push_back(toString(item));
    }

    return fmt::format("{}", fmt::join(items, toString(std::forward<String>(separator))));
}

} // namespace traces

#endif // TRACES_TOSTRING_H_INCLUDED
