cmake_minimum_required (VERSION 2.6)
project (making_it_to_steam)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -pedantic")
set(CMAKE_VERBOSE_MAKEFILE ON)
include_directories("./include")

if(APPLE)
    include_directories("/usr/local/include")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L/usr/local/lib")
endif()

FILE(GLOB_RECURSE headers
"*.hpp"
"*.h"
)
add_custom_target(headers SOURCES ${headers})

add_definitions(-DGLM_FORCE_NO_CTOR_INIT)

if(NOT CMAKE_BUILD_TYPE)
set (CMAKE_BUILD_TYPE Release)
endif()

add_subdirectory (src)
add_subdirectory (examples/test_scene)
add_subdirectory (game/anim_creator)
add_subdirectory (game/particle_editor)
