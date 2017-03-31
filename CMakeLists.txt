cmake_minimum_required (VERSION 2.6)
project (making_it_to_steam)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -pedantic")
include_directories("./include")

FILE(GLOB_RECURSE headers
"*.hpp"
"*.h"
)
add_custom_target(headers SOURCES ${headers})

add_subdirectory (src)
add_subdirectory (examples/test_scene)
add_subdirectory (game/anim_creator)
