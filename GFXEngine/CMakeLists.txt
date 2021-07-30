cmake_minimum_required(VERSION 3.16)
project(GFXEngine)

set(CMAKE_CXX_STANDARD 17)

set(CURSES_NEED_NCURSES true)

find_library(CARBON Carbon)

find_package(Curses)

find_package(X11)

include_directories(${X11_INCLUDE_DIR} ${CURSES_INCLUDE_DIRS})

add_executable(GFXEngine main.cpp ../NCursesGameEngine.h ../GFXToolkit.h)

target_link_libraries(GFXEngine ${X11_LIBRARIES} ${CARBON} ${CURSES_LIBRARIES})
