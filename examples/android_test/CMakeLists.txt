set (android_test_SRC

main.cpp
android_scene.hpp
)

add_executable (android_test ${android_test_SRC})
target_link_libraries(android_test mits)
