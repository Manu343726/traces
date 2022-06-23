cmake_minimum_required(VERSION 3.0.0)
project(traces)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
set(CMAKE_CXX_STANDARD 17)

option(TRACES_ENABLED "Enable profiling traces" ON)
option(TRACES_ON_DEMAND "Enable profiling on demand" ON)
option(TRACES_NO_EXIT "Keep application running until all profiling traces are collected by a tracing server" OFF)
option(TRACES_DISABLE_BROADCAST "Disable broadcasting of client in local network" OFF)
option(TRACES_DISABLE_STACK_TRACES "Disable stack traces" OFF)
option(TRACES_DISABLE_MEMORY_ALLOCATION_TRACES "Disable tracing of memory allocations" OFF)
option(TRACES_DISABLE_MUTEX_TRACES "Disable tracing of mutexes" OFF)
option(TRACES_FMT_HEADER_ONLY "Use header-only version of fmt library" OFF)

if(NOT TRACES_PORT)
    set(TRACES_PORT 8086)
endif()

if(NOT TRACES_STACKTRACE_DEPTH)
    set(TRACES_STACKTRACE_DEPTH 100)
endif()

if(NOT TRACES_MEMORY_STACKTRACE_DEPTH)
    set(TRACES_MEMORY_STACKTRACE_DEPTH 30)
endif()

if(NOT TRACES_MODULES)
    set(TRACES_MODULES)
endif()

set(TRACES_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TRACES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(TRACES_HEADERS
    ${TRACES_INCLUDE_DIR}/traces/traces.h
    ${TRACES_INCLUDE_DIR}/traces/tracemacros.h
    ${TRACES_INCLUDE_DIR}/traces/threadname.h
    ${TRACES_INCLUDE_DIR}/traces/eventtracer.h
    ${TRACES_INCLUDE_DIR}/traces/functionname.h
    ${TRACES_INCLUDE_DIR}/traces/color.h
    ${TRACES_INCLUDE_DIR}/traces/modules.h
    ${TRACES_INCLUDE_DIR}/traces/tostring.h
)

set(TRACES_SOURCES
    ${TRACES_SOURCE_DIR}/eventtracer.cpp
)

add_library(traces
    ${TRACES_HEADERS}
    ${TRACES_SOURCES}
)

target_compile_definitions(traces PUBLIC HAS_TRACES)
target_include_directories(traces PUBLIC ${TRACES_INCLUDE_DIR})

find_package(tracy_client REQUIRED)
find_package(fmt REQUIRED)

target_link_libraries(traces PUBLIC tracy_client::tracy_client)

if(TRACES_FMT_HEADER_ONLY)
    target_link_libraries(traces PUBLIC fmt::fmt-header-only)
else()
    target_link_libraries(traces PUBLIC fmt::fmt)
endif()

if(TRACES_DISABLE_MEMORY_ALLOCATION_TRACES)
    message(STATUS "traces memory allocation traces disabled")
    target_compile_definitions(traces PUBLIC TRACES_DISABLE_MEMORY_ALLOCATION_TRACES)
endif()

if(TRACES_DISABLE_MUTEX_TRACES)
    message(STATUS "traces mutex traces disabled")
    target_compile_definitions(traces PUBLIC TRACES_DISABLE_MUTEX_TRACES)
endif()

if(TRACES_DISABLE_STACK_TRACES)
    message(STATUS "traces stack traces disabled")
    target_compile_definitions(traces PUBLIC TRACES_DISABLE_STACK_TRACES)
endif()

target_compile_definitions(traces PUBLIC TRACES_STACKTRACE_DEPTH=${TRACES_STACKTRACE_DEPTH})
target_compile_definitions(traces PUBLIC TRACES_MEMORY_STACKTRACE_DEPTH=${TRACES_MEMORY_STACKTRACE_DEPTH})

include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
    #include <string_view>

    constexpr std::string_view parse(std::string_view input)
    {
        if (!input.empty() && input[0] == 'a')
        {
            return input.substr(1);
        }
        else
        {
            return input;
        }
    }

    int main()
    {
        constexpr std::string_view name = parse(__PRETTY_FUNCTION__);
    }
"
    TRACES_HAS_CONSTEXPR_PRETTY_FUNCTION)

check_cxx_source_compiles("
    #include <string_view>

    int main()
    {
        constexpr std::string_view hello;
        constexpr auto found = hello.find_first_of('h');
    }
"
TRACES_HAS_CONSTEXPR_STRING_VIEW)

if(TRACES_HAS_CONSTEXPR_PRETTY_FUNCTION)
    target_compile_definitions(traces PUBLIC TRACES_HAS_CONSTEXPR_PRETTY_FUNCTION)
endif()

if(TRACES_HAS_CONSTEXPR_STRING_VIEW)
    target_compile_definitions(traces PUBLIC TRACES_HAS_CONSTEXPR_STRING_VIEW)
endif()

if(BUILD_SHARED_LIBS)
    install(TARGETS traces LIBRARY DESTINATION lib)
else()
    install(TARGETS traces ARCHIVE DESTINATION lib)
endif()
