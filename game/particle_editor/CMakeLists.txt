set (particle_editor_SRCS

main.cpp
particle_editor.cpp
emitter.cpp
)

add_executable(particle_editor ${particle_editor_SRCS})
target_link_libraries(particle_editor mits)
file(COPY res DESTINATION .)
