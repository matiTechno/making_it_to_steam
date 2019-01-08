# making_it_to_steam

[youtube video](https://youtu.be/fEQpRWuxpLY)

[screenshots](https://github.com/matiTechno/making_it_to_steam/issues/3)

2d game (now it's only graphics engine)

dependencies:  
SDL2  
SDL2_Mixer  
FreeType  
glm  
OpenGL 3.3+  
  
included in source:  
ImGui  
stb_image  
glad (link with -ldl on unix)  
  
documented in source  
see examples

works with cmake only on linux (most simplest CMakeLists.txt ever)

##### update:

on mac os: install homebrew (package manager for mac)  
then:  
brew install sdl2 sdl2_mixer --with-libvorbis freetype glm  
and now compilation should succeed
