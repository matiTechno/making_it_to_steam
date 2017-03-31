set (test_scene_SRC

main.cpp
snake1.cpp
snake1_end_menu.cpp
test_scene.cpp
)

add_executable (test_scene ${test_scene_SRC})
target_link_libraries(test_scene mits -lSDL2)
file(COPY res DESTINATION .)
file(COPY shaders DESTINATION .)
