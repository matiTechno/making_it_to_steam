set (SRCS

app.cpp
scene.cpp
sound_system.cpp

glad/glad.c

imgui/imgui.cpp
imgui/imgui_demo.cpp
imgui/imgui_draw.cpp
imgui/imgui_impl_sdl_gl3.cpp

opengl/buffers.cpp
opengl/other.cpp
opengl/shader.cpp
opengl/texture.cpp

rendering/font_loader.cpp
rendering/postprocessor.cpp
rendering/renderer_2d.cpp
rendering/text.cpp
)

if(NOT CMAKE_BUILD_TYPE)
set (CMAKE_BUILD_TYPE Release)
endif()

FILE(GLOB_RECURSE shaders
    "rendering/shaders/*"
)

add_custom_target(shaders SOURCES ${shaders})

include_directories("/usr/include/freetype2")
add_library(mits SHARED ${SRCS})
target_link_libraries(mits -lfreetype -lSDL2_mixer -lSDL2 -lGL -ldl -lgcc_s -lgcc)
