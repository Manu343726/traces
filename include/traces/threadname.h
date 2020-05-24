#ifndef TRACES_THREADNAME_H_INCLUDED
#define TRACES_THREADNAME_H_INCLUDED

#include "tostring.h"
#include <common/TracySystem.hpp>

namespace traces
{

template<typename String, typename... Args>
void setThreadName(String&& body, Args&&... args)
{
    const auto name = ::traces::toString(
        std::forward<String>(body), std::forward<Args>(args)...);

    ::tracy::SetThreadName(name.c_str());
}
} // namespace traces

#endif // TRACES_THREADNAME_H_INCLUDED
