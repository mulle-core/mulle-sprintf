cmake_minimum_required( VERSION 3.0)

project( mulle-sprintf C)


### mulle-sde environment

# add cmake module paths to search path
list( INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake/share")
list( INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake")

include( Environment)


### Files

include( PreFiles OPTIONAL)

set( MULLE_SPRINTF_LIBRARY ON)

#
# Include all non underscored .cmake files in ./cmake
#
file( GLOB LOCAL_CMAKE_INCLUDES
   "cmake/[A-Za-z0-9]*.cmake"
)

foreach( FILE ${LOCAL_CMAKE_INCLUDES} )
   include( "${FILE}")
endforeach()


include_directories( ${INCLUDE_DIRS})

include( PostFiles OPTIONAL)

### Library

include( PreLibrary OPTIONAL)

add_library( "mulle-sprintf"
# definitions from cmake/HeadersAndSources.cmake
${SOURCES}
${PUBLIC_HEADERS}
${PRIVATE_HEADERS}
# definitions from cmake/share/Environment.cmake
${CMAKE_EDITABLE_FILES}
)



if( BUILD_SHARED_LIBS)
   target_link_libraries( mulle-sprintf
# definitions from cmake/share/DependenciesAndLibraries.cmake
${DEPENDENCY_LIBRARIES}
${OS_SPECIFIC_LIBRARIES}
)
endif()

include( PostLibrary OPTIONAL)

option( STANDALONE "Create standalone library for debugging" OFF)

set( INSTALL_LIBRARY_TARGETS
"mulle-sprintf"
)

if( STANDALONE)
   set( STANDALONE_BASE_NAME "mulle-sprintf")
   set( STANDALONE_NAME "mulle-sprintf-standalone")
   set( STANDALONE_DEFINITIONS ${MULLE_SPRINTF_DEFINITIONS})
   set( STANDALONE_DEPENDENCIES
   mulle-sprintf
   )

   set( STANDALONE_ALL_LOAD_LIBRARIES
   $<TARGET_FILE:mulle-sprintf>
   ${DEPENDENCY_LIBRARIES}
   ${OPTIONAL_DEPENDENCY_LIBRARIES}
   ${OS_SPECIFIC_LIBRARIES}
   )

   include( Standalone)

   set( INSTALL_LIBRARY_TARGETS
   ${INSTALL_LIBRARY_TARGETS}
   "mulle-sprintf-standalone"
   )
endif()

### Install

include( PreInstall OPTIONAL)

install( TARGETS ${INSTALL_LIBRARY_TARGETS} DESTINATION "lib")
install( FILES ${PUBLIC_HEADERS}  DESTINATION "include/mulle-sprintf")
install( FILES ${PRIVATE_HEADERS} DESTINATION "include/mulle-sprintf/private")

# definitions from cmake/share/Environment.cmake
install( FILES ${CMAKE_INCLUDES}  DESTINATION "include/mulle-sprintf/cmake")

# definitions from cmake/share/LinkManifest.cmake
install( FILES ${LINK_MANIFEST_FILES}  DESTINATION "include/mulle-sprintf/link")

include( PostInstall OPTIONAL)
