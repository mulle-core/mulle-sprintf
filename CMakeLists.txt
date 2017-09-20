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

set( MULLE_SPRINTF_LIBRARY ON)
include( CMakeSourcesAndHeaders.txt)
include( CMakeDependencies.txt)


set( PUBLIC_HEADERS
${HEADERS}
CMakeDependencies.txt
)


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


set( STANDALONE_BASE_NAME "mulle_sprintf")
set( STANDALONE_NAME "mulle_sprintf_standalone")
set( STANDALONE_DEFINITIONS ${MULLE_SPRINTF_DEFINITIONS})
set( STANDALONE_ALL_LOAD_LIBRARIES
$<TARGET_FILE:mulle_sprintf>
${DEPENDENCY_LIBRARIES}
)

include( Standalone)

INSTALL( TARGETS mulle_sprintf_standalone mulle_sprintf DESTINATION "lib")
INSTALL( FILES ${PUBLIC_HEADERS} DESTINATION "include/mulle_sprintf")

include( MulleTest)
