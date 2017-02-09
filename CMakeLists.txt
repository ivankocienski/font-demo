cmake_minimum_required (VERSION 2.6)

# our exe name
project (font-demo)

# for finding packages with pkg-config
find_package(PkgConfig REQUIRED)

# this gives us warnings about fishy code (-Wall)
#  and symbol tables (-dynamic) and gdb support (-g)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -g")

# makefile will print out what it is doing
set(CMAKE_VERBOSE_MAKEFILE ON)

# a hack for SDL
ENABLE_LANGUAGE(C)

# for SDL2
pkg_search_module(SDL2 REQUIRED sdl2)

# source files. sure, there's a better way of doing this
set (SRC
  src/main.cc
)

add_executable(font-demo ${SRC})

target_link_libraries(
  font-demo
  ${SDL2_LIBRARIES}
)


add_custom_command(TARGET font-demo POST_BUILD 
  COMMAND ln -sf ${CMAKE_SOURCE_DIR}/data
  ${CMAKE_CURRENT_BINARY_DIR}/data COMMENT "linking data directory"
)
