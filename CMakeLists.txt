cmake_minimum_required (VERSION 3.0)

project( mulle-sprintf C)


set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/mulle-configuration/cmake)
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

include( CMakeSourcesAndHeaders.txt)


find_library( MULLE_ALLOCATOR_LIBRARY      NAMES mulle_allocator)
message(STATUS "MULLE_ALLOCATOR_LIBRARY is ${MULLE_ALLOCATOR_LIBRARY}")
find_library( MULLE_BUFFER_LIBRARY         NAMES mulle_buffer)
message(STATUS "MULLE_BUFFER_LIBRARY is ${MULLE_BUFFER_LIBRARY}")
find_library( MULLE_TEST_ALLOCATOR_LIBRARY NAMES mulle_test_allocator)
message(STATUS "MULLE_TEST_ALLOCATOR_LIBRARY is ${MULLE_TEST_ALLOCATOR_LIBRARY}")
find_library( MULLE_THREAD_LIBRARY         NAMES mulle_thread)
message(STATUS "MULLE_THREAD_LIBRARY is ${MULLE_THREAD_LIBRARY}")
find_library( MULLE_UTF_LIBRARY            NAMES mulle_utf)
message(STATUS "MULLE_UTF_LIBRARY is ${MULLE_UTF_LIBRARY}")


set( DEPENDENCY_LIBRARIES
${MULLE_ALLOCATOR_LIBRARY}
${MULLE_BUFFER_LIBRARY}
${MULLE_TEST_ALLOCATOR_LIBRARY}
${MULLE_THREAD_LIBRARY}
${MULLE_UTF_LIBRARY}
)


add_library( mulle_sprintf
${SOURCES}
)

set( TARGET_ALL_LOAD_LIBRARIES
$<TARGET_FILE:mulle_sprintf>
${DEPENDENCY_LIBRARIES}
)

set( STANDALONE_BASE_NAME "mulle_sprintf")
set( STANDALONE_NAME "mulle_sprintf_standalone")

include( Standalone)


INSTALL( TARGETS mulle_sprintf_standalone mulle_sprintf DESTINATION "lib")
INSTALL( FILES ${HEADERS} DESTINATION "include/mulle_sprintf")
