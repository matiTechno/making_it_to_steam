TEMPLATE = app
CONFIG += console c++14 strict_c++
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -pedantic

DEFINES += GLM_FORCE_NO_CTOR_INIT USE_IMGUI

LIBS += -lGL -lfreetype -ldl -lSDL2_mixer -lSDL2

INCLUDEPATH += /usr/include/freetype2

DISTFILES += \
    shaders/shader_2d.frag \
    shaders/shader_fb_blend.frag \
    shaders/shader_fb_blur.frag \
    shaders/shader_2d.vert \
    shaders/shader_fb.vert \
    shaders/shader_2d_batching.frag \
    shaders/shader_2d_batching.vert \
    shaders/shader_fb_test_red.frag \
    shaders/shader_fb_final.frag

HEADERS += \
    source/app.hpp \
    source/key_input.hpp \
    source/glad/glad.h \
    source/opengl/base.hpp \
    source/opengl/buffers.hpp \
    source/opengl/shader.hpp \
    source/opengl/stb_image.h \
    source/opengl/texture.hpp \
    source/rendering/font_loader.hpp \
    source/rendering/postprocessor.hpp \
    source/rendering/renderer_2d.hpp \
    source/rendering/render_obj_base.hpp \
    source/rendering/sprite.hpp \
    source/rendering/text.hpp \
    source/imgui/imconfig.h \
    source/imgui/imgui.h \
    source/imgui/imgui_impl_sdl_gl3.h \
    source/imgui/imgui_internal.h \
    source/imgui/stb_rect_pack.h \
    source/imgui/stb_textedit.h \
    source/imgui/stb_truetype.h \
    source/test_scene/test_scene.hpp \
    source/res_class.hpp \
    source/scene.hpp \
    source/sound_system.hpp \
    source/systems.hpp

SOURCES += \
    source/app.cpp \
    source/main.cpp \
    source/glad/glad.c \
    source/opengl/buffers.cpp \
    source/opengl/shader.cpp \
    source/opengl/texture.cpp \
    source/rendering/font_loader.cpp \
    source/rendering/postprocessor.cpp \
    source/rendering/renderer_2d.cpp \
    source/rendering/text.cpp \
    source/sound_system.cpp \
    source/scene.cpp \
    source/test_scene/test_scene.cpp \
    source/imgui/imgui.cpp \
    source/imgui/imgui_demo.cpp \
    source/imgui/imgui_draw.cpp \
    source/imgui/imgui_impl_sdl_gl3.cpp
