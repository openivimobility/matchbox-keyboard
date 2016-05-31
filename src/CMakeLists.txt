find_package(X11 REQUIRED COMPONENTS X11)
find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)
find_package(EXPAT REQUIRED)
find_package(PkgConfig)
pkg_check_modules(cairo cairo)

include_directories(${X11_X11_INCLUDE_PATH})
include_directories(${X11_Xft_INCLUDE_PATH})
include_directories(${cairo_INCLUDE_DIRS})
include_directories(${Freetype_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIR})

add_definitions(-DHAVE_CONFIG_H)



# for config.h.in
set(PKGDATADIR \"${DATADIR}\")

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set (LIBRARY_SRC_FILES
  libmatchbox-keyboard.c
  matchbox-keyboard.c
  matchbox-keyboard-layout.c
  matchbox-keyboard-row.c
  matchbox-keyboard-key.c
  matchbox-keyboard-ui.c
  config-parser.c
  util-list.c
  util.c
)

if(CAIRO_FOUND)
  list(APPEND LIBRARY_SRC_FILES
    matchbox-keyboard-ui-cairo-backend.c
    matchbox-keyboard-popup.c)
else()
  list(APPEND LIBRARY_SRC_FILES
    matchbox-keyboard-ui-xft-backend.c
    matchbox-keyboard-image.c)
endif(CAIRO_FOUND)


add_library(
  libmatchbox-keyboard STATIC
  ${LIBRARY_SRC_FILES}
)

set(MAIN_SRC_FILES
  main.c
  matchbox-keyboard-layout.c
  matchbox-keyboard-xembed.c
  matchbox-keyboard-remote.c
  matchbox-keyboard-remote.h
)

add_executable(matchbox-keyboard ${MAIN_SRC_FILES})
target_link_libraries(matchbox-keyboard
  libmatchbox-keyboard
  fakekey
  ${X11_X11_LIB}
  ${X11_Xft_LIB}
  ${X11_Xrender_LIB}
  ${PNG_LIBRARIES}
  ${Freetype_LIBRARIES}
  ${EXPAT_LIBRARY}
)


install(TARGETS matchbox-keyboard RUNTIME DESTINATION bin)
