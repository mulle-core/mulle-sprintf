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
src/mulle_sprintf_function.h
src/functions/mulle_sprintf_character.h
src/functions/mulle_sprintf_escape.h
src/functions/mulle_sprintf_fp.h
src/functions/mulle_sprintf_integer.h
src/functions/mulle_sprintf_pointer.h
src/functions/mulle_sprintf_return.h
src/functions/mulle_sprintf_string.h
)


add_library(mulle_sprintf
src/mulle_sprintf.c
src/mulle_sprintf_function.c
src/functions/mulle_sprintf_character.c
src/functions/mulle_sprintf_escape.c
src/functions/mulle_sprintf_fp.c
src/functions/mulle_sprintf_integer.c
src/functions/mulle_sprintf_pointer.c
src/functions/mulle_sprintf_return.c
src/functions/mulle_sprintf_string.c
)


add_library( mulle_sprintf_standalone SHARED
src/mulle_sprintf_standalone.c
)

add_dependencies( mulle_sprintf_standalone mulle_sprintf)

find_library( MULLE_ALLOCATOR_LIBRARY      NAMES mulle_allocator)
find_library( MULLE_BUFFER_LIBRARY         NAMES mulle_buffer)
find_library( MULLE_TEST_ALLOCATOR_LIBRARY NAMES mulle_test_allocator)
find_library( MULLE_THREAD_LIBRARY         NAMES mulle_thread)
find_library( MULLE_UTF_LIBRARY            NAMES mulle_utf)

message(STATUS "MULLE_ALLOCATOR_LIBRARY is ${MULLE_ALLOCATOR_LIBRARY}")
message(STATUS "MULLE_BUFFER_LIBRARY is ${MULLE_BUFFER_LIBRARY}")
message(STATUS "MULLE_TEST_ALLOCATOR_LIBRARY is ${MULLE_TEST_ALLOCATOR_LIBRARY}")
message(STATUS "MULLE_THREAD_LIBRARY is ${MULLE_THREAD_LIBRARY}")
message(STATUS "MULLE_UTF_LIBRARY is ${MULLE_UTF_LIBRARY}")


set( DEPENDENCY_LIBRARIES
${MULLE_ALLOCATOR_LIBRARY}
${MULLE_BUFFER_LIBRARY}
${MULLE_TEST_ALLOCATOR_LIBRARY}
${MULLE_THREAD_LIBRARY}
${MULLE_UTF_LIBRARY}
)

TARGET_LINK_LIBRARIES( mulle_sprintf_standalone
${BEGIN_ALL_LOAD}
mulle_sprintf
${DEPENDENCY_LIBRARIES}

${END_ALL_LOAD}
)



INSTALL(TARGETS mulle_sprintf_standalone mulle_sprintf DESTINATION "lib")
INSTALL(FILES ${HEADERS} DESTINATION "include/mulle_sprintf")

