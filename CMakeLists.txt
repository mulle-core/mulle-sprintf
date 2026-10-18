cmake_minimum_required( VERSION 3.13...99.99)

project( mulle-sprintf VERSION 4.1.1 LANGUAGES C)

#
# The library advertises C11 (see the "std" badge) and POSIX.1-2008 (see the
# "apis: posix/windows" badge). Compile it as C11 with extensions off and ask
# for exactly that namespace: _POSIX_C_SOURCE=200809L gives POSIX.1-2008 and
# hides X/Open, BSD/SVID and GNU extensions, so anything beyond the badge is a
# compile error rather than a silent ratchet.
#
#
# On Apple, _DARWIN_C_SOURCE is the counterpart of glibc's _DEFAULT_SOURCE:
# strict C mode hides the POSIX and BSD declarations there, including the
# SO_NOSIGPIPE socket option this library sets on macOS.
#
# A standard given on the command line wins, so other dialects remain testable
# with e.g. -DCMAKE_C_STANDARD=23 or CFLAGS=-std=c23.
#
if( NOT CMAKE_C_STANDARD
    AND NOT CMAKE_C_FLAGS MATCHES "(^| )-std="
    AND NOT "$ENV{CFLAGS}" MATCHES "(^| )-std=")
   set( CMAKE_C_STANDARD 11)
   set( CMAKE_C_EXTENSIONS OFF)
   if( APPLE)
      add_compile_definitions( _DARWIN_C_SOURCE)
   elseif( NOT WIN32)
      add_compile_definitions( _POSIX_C_SOURCE=200809L)
   endif()
endif()


### mulle-sde environment

# add cmake module paths to search path
list( INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/share")
list( INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/reflect")
list( INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include( Environment)

include( Files)

### Library

if( HEADERS_PHASE)
   install( FILES ${INSTALL_PUBLIC_HEADERS}  DESTINATION "include/mulle-sprintf")
   install( FILES ${INSTALL_PRIVATE_HEADERS} DESTINATION "include/mulle-sprintf")
   install( FILES ${INSTALL_CMAKE_INCLUDES}  DESTINATION "include/mulle-sprintf/cmake")

   # short cut out
   if( NOT COMPILE_PHASE AND NOT LINK_PHASE)
      return()
   endif()
endif()

if( LINK_PHASE)
   include( Dependencies OPTIONAL)
endif()

include( Library)

# use LIBRARY_COMPILE_TARGET for compiler options
target_compile_definitions( "${LIBRARY_COMPILE_TARGET}" PUBLIC)
target_compile_options( "${LIBRARY_COMPILE_TARGET}" PUBLIC)
if( NOT MSVC)
   target_compile_options( "${LIBRARY_COMPILE_TARGET}" PUBLIC -Wunused)
endif()

# use LIBRARY_LINK_TARGET for linker options
if( LINK_PHASE)
   target_link_options( "${LIBRARY_LINK_TARGET}" PUBLIC)
endif()

include( InstallLibrary)

if( LINK_PHASE)
   include( AddSubdirectorySupport)
   include( FindPackageSupport)
endif() 

include( FinalOutput OPTIONAL)
