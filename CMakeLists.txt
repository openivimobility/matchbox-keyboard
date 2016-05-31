
cmake_minimum_required(VERSION 2.8.8)
project(matchbox-keyboard)
set(VERSION 0.1)

set(DATADIR ${CMAKE_INSTALL_LIBDIR}/matchbox-keyboard)
set(datadir ${DATADIR})

add_subdirectory(applet)
add_subdirectory(gtk-im)
add_subdirectory(layouts)
add_subdirectory(src)
