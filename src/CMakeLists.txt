
# TK2000 executable
add_executable (tk2000
  main.cpp Common.h Device.h Observer.h Subject.h
  pch.cpp pch.h
  Audio.cpp Audio.h
  AudioSDL.cpp AudioSDL.h
  Bus.cpp Bus.h
  Cpu6502.cpp Cpu6502.h
  Keyboard.cpp Keyboard.h
  Machine.cpp Machine.h
  Menu.cpp Menu.h
  Ram.cpp Ram.h
  Rom.cpp Rom.h
  Tape.cpp Tape.h
  Video.cpp Video.h
  WindowSDL.cpp WindowSDL.h
 "media.h")

# Precompiled headers
target_precompile_headers(tk2000 PUBLIC pch.h)
if (UNIX)
  target_link_libraries(tk2000 pthread)
endif()

target_link_libraries(tk2000 ${SDLGUI_EXTRA_LIBS} ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2TTF_LIBRARY})
