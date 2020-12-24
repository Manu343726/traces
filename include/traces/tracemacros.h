#ifndef TRACES_TRACEMACROS_H_INCLUDED
#define TRACES_TRACEMACROS_H_INCLUDED

#include "config.h"

#ifndef TRACY_ENABLE
#define TRACES_FUNCTION(...)
#define TRACES_FUNCTION_NAMED(...)
#define TRACES_ZONE(...)
#define TRACES_NAME(...)
#define TRACES_EVENT(...)
#define TRACES_EVENT_COLORED(color, ...)
#define TRACES_PLOT_NUMBER(value, ...)
#define TRACES_PLOT_MEMORY(value, ...)
#define TRACES_PLOT_PERCENT(value, ...)
#define TRACES_MEMORY_ALLOCATIONS()
#define TRACES_MUTEX(type, name) type name
#define TRACES_SHARED_MUTEX(type, name) type name
#define TRACES_MUTEX_NAMED(type, name, customName) type name
#define TRACES_SHARED_MUTEX_NAMED(type, name, customName) type name
#define TRACES_LOCK(name)
#else // TRACY_ENABLE

#include <algorithm>
#include <string>
#include <string_view>

#include <traces/color.h>
#include <traces/functionname.h>
#include <traces/tostring.h>

#ifdef _MSC_VER
#define TRACES_CURRENT_FUNCTION_NAME __FUNCSIG__
#elif defined(__GNUC__) or defined(__clang__)
#define TRACES_CURRENT_FUNCTION_NAME __PRETTY_FUNCTION__
#else
#define TRACES_CURRENT_FUNCTION_NAME __func__
#endif

#define TRACY_SOURCE_LOCATION_FUNCTION                   \
    ::traces::FunctionName(TRACES_CURRENT_FUNCTION_NAME) \
        .fullDisplayName()                               \
        .data()
#include <Tracy.hpp>
#include <TracyC.h>

#ifndef TRACES_TEXT_MAX_LENGTH
#define TRACES_TEXT_MAX_LENGTH 4096
#endif

#if not defined(TRACES_HAS_DEBUG_SYMBOLS) or \
    defined(TRACES_DISABLE_STACK_TRACES)
#define TRACES_ZONE_SCOPED(color) ZoneScopedC(color)
#define TRACES_ZONE_NAMED(variable, color) ZoneNamedC(variable, color, true)
#define TRACES_MESSAGE(cstr, length, color) TracyMessageC(cstr, length, color)
#define TRACES_ALLOC(ptr, size) TracyAlloc(ptr, size)
#define TRACES_FREE(ptr) TracyFree(ptr)
#else
#define TRACES_ZONE_SCOPED(color) ZoneScopedCS(color, TRACES_STACKTRACE_DEPTH)
#define TRACES_ZONE_NAMED(variable, color) \
    ZoneNamedCS(variable, color, TRACES_STACKTRACE_DEPTH, true)
#define TRACES_MESSAGE(cstr, length, color) \
    TracyMessageCS(cstr, length, color, TRACES_STACKTRACE_DEPTH)
#define TRACES_ALLOC(ptr, size) \
    TracyAllocS(ptr, size, TRACES_MEMORY_STACKTRACE_DEPTH)
#define TRACES_FREE(ptr) TracyFreeS(ptr, TRACES_MEMORY_STACKTRACE_DEPTH)
#endif // TRACES_HAS_DEBUG_SYMBOLS

#define TRACES_FUNCTION_NAMED(...)                                                             \
    TRACES_CONSTANT ::traces::detail::FunctionName                                             \
                                    __traces_trace_function_functionName{__PRETTY_FUNCTION__}; \
    TRACES_CONSTANT ::traces::Color __traces_trace_function_color =                            \
        ::traces::functionColor(__traces_trace_function_functionName);                         \
                                                                                               \
    TRACES_ZONE_SCOPED(__traces_trace_function_color);                                         \
                                                                                               \
    [&](const std::string& name) {                                                             \
        ZoneName(                                                                              \
            name.c_str(),                                                                      \
            std::min(                                                                          \
                name.length(),                                                                 \
                static_cast<std::size_t>(TRACES_TEXT_MAX_LENGTH)));                            \
    }(::traces::toString(__VA_ARGS__));                                                        \
                                                                                               \
    [&](auto&&... args) {                                                                      \
        const std::string text =                                                               \
            ::traces::toString(std::forward<decltype(args)>(args)...);                         \
                                                                                               \
        if(!text.empty())                                                                      \
        {                                                                                      \
            ZoneText(                                                                          \
                text.c_str(),                                                                  \
                std::min(                                                                      \
                    text.length(),                                                             \
                    static_cast<std::size_t>(TRACES_TEXT_MAX_LENGTH)));                        \
        }                                                                                      \
    }

#define TRACES_FUNCTION(...)                                                   \
    TRACES_CONSTANT ::traces::detail::FunctionName                             \
                    __traces_trace_function_functionName{__PRETTY_FUNCTION__}; \
    TRACES_CONSTANT std::string_view __traces_trace_function_name =            \
        __traces_trace_function_functionName.fullDisplayName();                \
    TRACES_CONSTANT ::traces::Color __traces_trace_function_color =            \
        ::traces::functionColor(__traces_trace_function_functionName);         \
                                                                               \
    TRACES_ZONE_SCOPED(__traces_trace_function_color);                         \
    ZoneName(                                                                  \
        __traces_trace_function_name.data(),                                   \
        std::min(                                                              \
            __traces_trace_function_name.size(),                               \
            static_cast<std::size_t>(TRACES_TEXT_MAX_LENGTH)));                \
                                                                               \
    const std::string __traces_trace_function_text =                           \
        ::traces::toString(__VA_ARGS__);                                       \
                                                                               \
    if(!__traces_trace_function_text.empty())                                  \
    {                                                                          \
        ZoneText(                                                              \
            __traces_trace_function_text.c_str(),                              \
            std::min(                                                          \
                __traces_trace_function_text.size(),                           \
                static_cast<std::size_t>(TRACES_TEXT_MAX_LENGTH)));            \
    }

#define TRACES_ZONE(...)                                                   \
    ZoneNamedCS(                                                           \
        TRACES_PP_CAT(__traces_trace_zone_, __LINE__),                     \
        __traces_trace_function_color,                                     \
        TRACES_STACKTRACE_DEPTH,                                           \
        true);                                                             \
    const std::string TRACES_PP_CAT(__traces_trace_zone_name_, __LINE__) = \
        ::traces::toString(__VA_ARGS__);                                   \
                                                                           \
    TRACES_PP_CAT(__traces_trace_zone_, __LINE__)                          \
        .Name(                                                             \
            TRACES_PP_CAT(__traces_trace_zone_name_, __LINE__).c_str(),    \
            std::min(                                                      \
                TRACES_PP_CAT(__traces_trace_zone_name_, __LINE__).size(), \
                static_cast<std::size_t>(TRACES_TEXT_MAX_LENGTH)))

#define TRACES_EVENT_COLORED(color, ...)                           \
    [](const auto& colorParam, const std::string& message) {       \
        TRACES_MESSAGE(                                            \
            message.c_str(),                                       \
            std::min(                                              \
                message.size(),                                    \
                static_cast<std::size_t>(TRACES_TEXT_MAX_LENGTH)), \
            static_cast<std::uint32_t>(colorParam));               \
    }(color, traces::toString(__VA_ARGS__))

#define TRACES_EVENT(...) \
    TRACES_EVENT_COLORED(::traces::Color::black(), __VA_ARGS__)

#define TRACES_PLOT_(...)                                           \
    [](const auto config, const auto value, auto&&... name) {       \
        []([[maybe_unused]] const auto         config,              \
           [[maybe_unused]] const std::string& name,                \
           [[maybe_unused]] const auto         value) {                     \
            [[maybe_unused]] const char* name_cstr = name.c_str();  \
            /*TracyPlotConfig(name_cstr, config);*/                 \
            /*TracyPlot(name_cstr, value);*/                        \
        }(config,                                                   \
          ::traces::toString(std::forward<decltype(name)>(name)...) \
              .substr(0, TRACES_TEXT_MAX_LENGTH),                   \
          value);                                                   \
    }(__VA_ARGS__);

#define TRACES_PLOT_NUMBER(...) \
    TRACES_PLOT_(::tracy::PlotFormatType::Number, __VA_ARGS__)

#define TRACES_PLOT_MEMORY(value, ...) \
    TRACES_PLOT_(::tracy::PlotFormatType::Memory, __VA_ARGS__)

#define TRACES_PLOT_PERCENT(value, ...) \
    TRACES_PLOT_(::tracy::PlotFormatType::Percentage, __VA_ARGS__)

#ifndef TRACES_DISABLE_MEMORY_ALLOCATION_TRACES
#define TRACES_MEMORY_ALLOCATIONS()          \
    void* operator new(std::size_t count)    \
    {                                        \
        auto ptr = std::malloc(count);       \
        TRACES_ALLOC(ptr, count);            \
        return ptr;                          \
    }                                        \
                                             \
    void operator delete(void* ptr) noexcept \
    {                                        \
        TRACES_FREE(ptr);                    \
        std::free(ptr);                      \
    }

#else
#define TRACES_MEMORY_ALLOCATIONS()
#endif // TRACES_DISABLE_MEMORY_ALLOCATION_TRACES

#ifndef TRACES_DISABLE_MUTEX_TRACES
#define TRACES_MUTEX(type, name) TracyLockable(type, name)
#define TRACES_SHARED_MUTEX(type, name) TracySharedLockable(type, name)
#define TRACES_MUTEX_NAMED(type, name, customName) \
    TracyLockableN(type, name, customName)
#define TRACES_SHARED_MUTEX_NAMED(type, name, customName) \
    TracySharedLockableN(type, name, customName)
#define TRACES_LOCK(name) LockMark(name)
#else
#define TRACES_MUTEX(type, name) type name
#define TRACES_SHARED_MUTEX(type, name) type name
#define TRACES_MUTEX_NAMED(type, name, customName) type name
#define TRACES_SHARED_MUTEX_NAMED(type, name, customName) type name
#define TRACES_LOCK(name)
#endif // TRACES_DISABLE_MUTEX_TRACES
#endif // TRACY_ENABLE
#endif // TRACES_TRACEMACROS_H_INCLUDED
