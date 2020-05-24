# Traces: Type safe Tracy API

## What is it?

This is a simple type-safe wrapper of the
[Tracy](https://github.com/wolfpld/tracy) client macros. It provides:

 - **Type safe trace name and text formatting** using
 [fmt](https://github.com/fmtlib/fmt).
 - **Integration with Qt and protobuf types**. Any type with any of the following
 string conversion methods is compatible:
   - `QString T::toString()`
   - `std::string T::toString()`
   - `std::string T::toStdString()`
   - `std::string T::DebugString()`

 - **Automatic coloring of captured frames and events based on source module**: Users list the
 different modules of the application, then the appropiate module and
 associated unique color is picked from the traced function name. A material
 palette is used. *All module selection and color picking is done at compile
 time if possible*.
 - **Improved captured function name**: Function traces use the full qualified name
 of the function by default. Overloading is also supported, the arguments are
 included in the name too. *Function name parsing is done at compile time if
 possible*.

 - **Predefined macro to set hooks for memory allocation tracing**.

 - **Automatic enable/disable of stacktrace capturing depending on available debug info**. *It can be force-disabled if needed*. 

 - **Global stacktrace capture depth configuration**.

 - **Hierarchical event tracing API**: Events are traced through an
 `EventTracer` API with an interface similar to a logger. These tracers can
 be built hierarchically and tagged with each module and submodule they
 belong to. Events are colored with their module color.

## Disclaimer

The wrapper is not nanosecond performance focused, but it is focused on
usability. This means some dynamic allocations are performed as part of the
formatting of the trace names and texts, which may not be suitable for
sub-microsecond real time applications. That said, the library tries to reduce
these allocation to the minimum needed to perform the required string
formatting. If you thing this can be improved while keeping the existing
functionality, please take a look at `tracemacros.h` and open an issue with any
suggestion or idea.

## Example

``` cpp
#define TRACES_MODULES "'main' 'gameplay', 'physics', 'rendering'"
#include <traces/traces.h>

namespace gameplay
{

traces::EventTracer traceEvent()
{
    return traces::traceEvent().tag("gameplay");
}

struct Player
{
    int id;

    Player(int id) : id{id}
    {
        // The zone is named "Load player", each capture
        // includes the player id in the capture text ("Loading player <id>")
        TRACES_ZONE_NAMED("Load player")("Loading player {}", id);

        ...
    }

    ~Player()
    {
        // An event with text "[gameplay][Player][<id>] Destroyed"
        // is traced
        traceEvent()("Destroyed");
    }
    
private:
    traces::EventTracer traceEvent() const
    {
        gameplay::traceEvent().tag("Player").tag("[{}]", id);
    }
};

void tick(Player& player)
{
    // The formatted string goes to the captured frame text ("Player: <id>")
    // The function is named "gameplay::tick(Player&)" in the captured frame
    TRACES_FUNCTION("Player: {}", player.id);

    ...
}

} // namespace gameplay

int main()
{
    while(true)
    {
        {
            TRACES_ZONE("game logic");

            for(auto& player : players)
            {
                gameplay::tick(player);
            }
        }

        {
            TRACES_ZONE("render");

            graphics::render(players);
        }
    }
}
```

## Install instructions

This project provides a CMake config script that requests the two dependencies,
tracy and fmt, through `find_package()` calls. This should be enough to
integrate the library into existing CMake projects.

On the other hand, the library is packaged and available through conan, using
my bintray repository:

```
conan remote add manu343726 https://api.bintray.com/conan/manu343726/conan-packages
conan install traces/0.0.1 --build=missing 
```

To enable/disable tracing the library uses the tracy config macros, which are
mapped to conan package options. Note tracing is enabled by default:

```
# conanfile.txt of your project:

[requires]
traces/0.0.1

[options]
tracy_client:TRACY_ENABLE=False

[generators]
cmake_find_package

```

## Documentation

Docu-what? Please refer to the example above or take a look into the
`include/traces/tracemacros.h` file. Feel free to open an issue with any
question you have.
