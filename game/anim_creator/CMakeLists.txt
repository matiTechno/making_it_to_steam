set (anim_creator_SRCS

anim_creator.cpp
main.cpp
camera_scene.cpp
anim_rect.cpp
origin_mode.cpp
preview.cpp
adjust_anims.cpp
set_coll_data.cpp
)

add_executable(anim_creator ${anim_creator_SRCS})
target_link_libraries(anim_creator mits)
file(COPY res DESTINATION .)
