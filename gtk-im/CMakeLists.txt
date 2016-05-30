find_package(GTK2 COMPONENTS)

include_directories(
  ${GTK2_INCLUDE_DIRS}
  ../src)

add_library(
  mb-im-invoker
  SHARED
  im-module.c
  im-context.h im-context.c
  im-protocol.h im-protocol.c
)

install(TARGETS mb-im-invoker LIBRARY DESTINATION lib/gtk-2.0/matchbox-keyboard/immodules)
