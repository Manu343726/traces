cmake_minimum_required(VERSION 3.0)
project(TracesExample)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

add_executable(traces_example main.cpp)
target_link_libraries(traces_example PRIVATE ${CONAN_LIBS})
