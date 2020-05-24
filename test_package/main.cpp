#include <string_view>

#define TRACES_MODULES "'main' 'module1' 'module1::submodule1'"
#include <traces/traces.h>

static_assert(traces::modules.size() == 3);
static_assert(traces::modules[0] == "main");
static_assert(traces::modules[1] == "module1");
static_assert(traces::modules[2] == "module1::submodule1");

namespace module1
{

traces::EventTracer traceEvent()
{
    return traces::traceEvent().tag("module{0}", 1);
}

void g(int i)
{
    TRACES_FUNCTION("i: {}", i);
    traceEvent()("g(i: {}) call", i);
}

namespace submodule1
{

traces::EventTracer traceEvent()
{
    return module1::traceEvent().tag("submodule{0}", 1);
}

void f(int i)
{
    TRACES_FUNCTION("i: {}", i);
    traceEvent()("f(i: {}) call", i);
    g(i);

    if(i > 0)
    {
        f(i - 1);
    }
}
} // namespace submodule1

} // namespace module1

int main()
{
    TRACES_FUNCTION_NAMED("main")("main function");

    for(const auto& module : traces::modules)
    {
        fmt::print("Found module \"{}\"\n", module);
    }

    for(int i = 0; i < 10000; ++i)
    {
        module1::submodule1::f(2);
        traces::traceEvent()("hello");
    }

#ifndef TRACY_ENABLE
    fmt::print("Tracy disabled!\n");
#endif
}
