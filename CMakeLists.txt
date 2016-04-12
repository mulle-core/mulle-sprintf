cmake_minimum_required (VERSION 3.0)

project (mulle-sprintf)

#
# dont forget to wipe all cmake produced files before
# changing this
#

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
   set (CMAKE_C_FLAGS "-Wno-parantheses")
endif()

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -DDEBUG -DMULLE_ATOMIC_TRACE")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g -DNDEBUG")

set(HEADERS
src/mulle_sprintf.h
src/mulle_standalone_sprintf.h
src/functions/mulle_sprintf_character.h
src/functions/mulle_sprintf_decimal.h
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
src/mulle_standalone_sprintf.c
src/functions/mulle_sprintf_character.c
src/functions/mulle_sprintf_decimal.c
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

