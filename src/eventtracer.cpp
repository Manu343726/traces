#include <traces/eventtracer.h>

using namespace traces;

EventTracer::EventTracer(std::string module, std::string prefix)
    : _module{std::move(module)}, _prefix{std::move(prefix)}
{
}

EventTracer EventTracer::subModule(std::string_view subModule) &&
{
    return {fmt::format("{}::{}", std::move(_module), subModule),
            std::move(_prefix)};
}

EventTracer traces::traceEvent()
{
    return EventTracer{""};
}
