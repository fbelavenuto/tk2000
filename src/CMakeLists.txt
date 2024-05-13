# Copyright (c) 2024 FBLabs
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# TK2000 executable
add_executable (tk2000 WIN32
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
