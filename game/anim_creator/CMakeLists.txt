set (anim_creator_SRCS

anim_creator.cpp
main.cpp
camera_scene.cpp
)

add_executable(anim_creator ${anim_creator_SRCS})
target_link_libraries(anim_creator mits)
file(COPY res DESTINATION .)
