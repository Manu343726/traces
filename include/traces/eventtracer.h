#ifndef TRACES_EVENTTRACER_H_INCLUDED
#define TRACES_EVENTTRACER_H_INCLUDED

#include <string>

#include "color.h"
#include "tostring.h"
#include "tracemacros.h"

namespace traces
{

class EventTracer
{
public:
    EventTracer(std::string module, std::string prefix = "");

    template<typename String, typename... Args>
    EventTracer tag(String&& body, Args&&... args) &&
    {
        return {
            std::move(_module),
            fmt::format(
                "{}[{}]",
                std::move(_prefix),
                ::traces::toString(
                    std::forward<String>(body), std::forward<Args>(args)...))};
    }

    EventTracer subModule(std::string_view subModule) &&;

    template<typename... Args>
    void operator()(Args&&... args) const
    {
        TRACES_EVENT_COLORED(
            ::traces::messageColor(_module),
            fmt::format(
                "{} {}",
                _prefix,
                ::traces::toString(std::forward<Args>(args)...)));
    }

private:
    std::string _module;
    std::string _prefix;
};

EventTracer traceEvent();

} // namespace traces

#endif // TRACES_EVENTTRACER_H_INCLUDED
