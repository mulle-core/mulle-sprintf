cmake_minimum_required (VERSION 3.0)

project (mulle-sprintf)


set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/mulle-configuration)
message(STATUS "CMAKE_MODULE_PATH is ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  include( Debug)
else()
  include( Release)
endif()

#
# dont forget to wipe all cmake produced files before
# changing this
#

include_directories(
src
src/functions
)

set(HEADERS
src/mulle_sprintf.h
src/functions/mulle_sprintf_character.h
src/functions/mulle_sprintf_escape.h
src/functions/mulle_sprintf_fp.h
src/functions/mulle_sprintf_function.h
src/functions/mulle_sprintf_int.h
src/functions/mulle_sprintf_pointer.h
src/functions/mulle_sprintf_return.h
src/functions/mulle_sprintf_string.h
)


add_library(mulle_sprintf
src/mulle_sprintf.c
src/functions/mulle_sprintf_character.c
src/functions/mulle_sprintf_escape.c
src/functions/mulle_sprintf_fp.c
src/functions/mulle_sprintf_function.c
src/functions/mulle_sprintf_int.c
src/functions/mulle_sprintf_pointer.c
src/functions/mulle_sprintf_return.c
src/functions/mulle_sprintf_string.c
)


INSTALL(TARGETS mulle_sprintf DESTINATION "lib")
INSTALL(FILES ${HEADERS} DESTINATION "include/mulle_sprintf")

