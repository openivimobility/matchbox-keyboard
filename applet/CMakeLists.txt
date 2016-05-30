
find_package(GTK2 COMPONENTS)

include_directories(
  ${GTK2_INCLUDE_DIRS}
  ../src
  ..)

add_library(
  keyboard
  SHARED
  applet.c ../gtk-im/im-protocol.c
)

install(TARGETS keyboard LIBRARY DESTINATION lib/matchbox-panel)
