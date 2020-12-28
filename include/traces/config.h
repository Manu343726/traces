#ifndef TRACES_CONFIG_H
#define TRACES_CONFIG_H

#if __has_include(<QString>)
#define TRACES_HAS_QSTRING
#include <QString>
#endif // has QString

#ifndef NDEBUG
#define TRACES_HAS_DEBUG_SYMBOLS
#endif // NDEBUG

#define TRACES_PP_CAT_(x, y) x##y
#define TRACES_PP_CAT(x, y) TRACES_PP_CAT_(x, y)

#ifdef TRACES_HAS_CONSTEXPR_PRETTY_FUNCTION
#define TRACES_CONSTANT constexpr
#define TRACY_USE_CONSTEXPR_VARIABLES
#else
#define TRACES_CONSTANT const
#endif // TRACES_HAS_CONSTEXPR_PRETTY_FUNCTION

#ifdef TRACES_HAS_CONSTEXPR_STRING_VIEW
#define TRACES_CONSTEXPR_FUNCTION constexpr
#define TRACES_CONSTEXPR_GLOBAL_VARIABLE constexpr
#else
#define TRACES_CONSTEXPR_FUNCTION inline
#define TRACES_CONSTEXPR_GLOBAL_VARIABLE inline const
#endif // TRACES_HAS_CONSTEXPR_STRING_VIEW

#ifndef TRACES_STACKTRACE_DEPTH
#define TRACES_STACKTRACE_DEPTH 100
#endif // TRACES_STACKTRACE_DEPTH

#ifndef TRACES_MEMORY_STACKTRACE_DEPTH
#define TRACES_MEMORY_STACKTRACE_DEPTH 30
#endif // TRACES_MEMORY_STACKTRACE_DEPTH

#endif // TRACES_CONFIG_H
