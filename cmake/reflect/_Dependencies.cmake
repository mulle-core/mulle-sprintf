# This file will be regenerated by `mulle-sourcetree-to-cmake` via
# `mulle-sde reflect` and any edits will be lost.
#
# This file will be included by cmake/share/Files.cmake
#
# Disable generation of this file with:
#
# mulle-sde environment set MULLE_SOURCETREE_TO_CMAKE_DEPENDENCIES_FILE DISABLE
#
if( MULLE_TRACE_INCLUDE)
   message( STATUS "# Include \"${CMAKE_CURRENT_LIST_FILE}\"" )
endif()

#
# Generated from sourcetree: 4C9D630E-7484-4392-A3DD-E021EDAE3A4A;mulle-buffer;no-all-load,no-cmake-searchpath,no-import,no-singlephase;
# Disable with : `mulle-sourcetree mark mulle-buffer no-link`
# Disable for this platform: `mulle-sourcetree mark mulle-buffer no-cmake-platform-linux`
#
if( NOT MULLE_BUFFER_LIBRARY)
   find_library( MULLE_BUFFER_LIBRARY NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-buffer${CMAKE_STATIC_LIBRARY_SUFFIX} mulle-buffer NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
   message( STATUS "MULLE_BUFFER_LIBRARY is ${MULLE_BUFFER_LIBRARY}")
   #
   # The order looks ascending, but due to the way this file is read
   # it ends up being descending, which is what we need.
   #
   if( MULLE_BUFFER_LIBRARY)
      #
      # Add MULLE_BUFFER_LIBRARY to DEPENDENCY_LIBRARIES list.
      # Disable with: `mulle-sourcetree mark mulle-buffer no-cmake-add`
      #
      set( DEPENDENCY_LIBRARIES
         ${DEPENDENCY_LIBRARIES}
         ${MULLE_BUFFER_LIBRARY}
         CACHE INTERNAL "need to cache this"
      )
      #
      # Inherit information from dependency.
      # Encompasses: no-cmake-searchpath,no-cmake-dependency,no-cmake-loader
      # Disable with: `mulle-sourcetree mark mulle-buffer no-cmake-inherit`
      #
      # temporarily expand CMAKE_MODULE_PATH
      get_filename_component( _TMP_MULLE_BUFFER_ROOT "${MULLE_BUFFER_LIBRARY}" DIRECTORY)
      get_filename_component( _TMP_MULLE_BUFFER_ROOT "${_TMP_MULLE_BUFFER_ROOT}" DIRECTORY)
      #
      #
      # Search for "DependenciesAndLibraries.cmake" to include.
      # Disable with: `mulle-sourcetree mark mulle-buffer no-cmake-dependency`
      #
      foreach( _TMP_MULLE_BUFFER_NAME "mulle-buffer")
         set( _TMP_MULLE_BUFFER_DIR "${_TMP_MULLE_BUFFER_ROOT}/include/${_TMP_MULLE_BUFFER_NAME}/cmake")
         # use explicit path to avoid "surprises"
         if( EXISTS "${_TMP_MULLE_BUFFER_DIR}/DependenciesAndLibraries.cmake")
            unset( MULLE_BUFFER_DEFINITIONS)
            list( INSERT CMAKE_MODULE_PATH 0 "${_TMP_MULLE_BUFFER_DIR}")
            # we only want top level INHERIT_OBJC_LOADERS, so disable them
            if( NOT NO_INHERIT_OBJC_LOADERS)
               set( NO_INHERIT_OBJC_LOADERS OFF)
            endif()
            list( APPEND _TMP_INHERIT_OBJC_LOADERS ${NO_INHERIT_OBJC_LOADERS})
            set( NO_INHERIT_OBJC_LOADERS ON)
            #
            include( "${_TMP_MULLE_BUFFER_DIR}/DependenciesAndLibraries.cmake")
            #
            list( GET _TMP_INHERIT_OBJC_LOADERS -1 NO_INHERIT_OBJC_LOADERS)
            list( REMOVE_AT _TMP_INHERIT_OBJC_LOADERS -1)
            #
            list( REMOVE_ITEM CMAKE_MODULE_PATH "${_TMP_MULLE_BUFFER_DIR}")
            set( INHERITED_DEFINITIONS
               ${INHERITED_DEFINITIONS}
               ${MULLE_BUFFER_DEFINITIONS}
               CACHE INTERNAL "need to cache this"
            )
            break()
         else()
            message( STATUS "${_TMP_MULLE_BUFFER_DIR}/DependenciesAndLibraries.cmake not found")
         endif()
      endforeach()
   else()
      # Disable with: `mulle-sourcetree mark mulle-buffer no-require-link`
      message( FATAL_ERROR "MULLE_BUFFER_LIBRARY was not found")
   endif()
endif()


#
# Generated from sourcetree: CD4D015D-20FB-456F-B8CD-43CEEB5DCA19;mulle-utf;no-all-load,no-cmake-searchpath,no-import,no-singlephase;
# Disable with : `mulle-sourcetree mark mulle-utf no-link`
# Disable for this platform: `mulle-sourcetree mark mulle-utf no-cmake-platform-linux`
#
if( NOT MULLE_UTF_LIBRARY)
   find_library( MULLE_UTF_LIBRARY NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-utf${CMAKE_STATIC_LIBRARY_SUFFIX} mulle-utf NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
   message( STATUS "MULLE_UTF_LIBRARY is ${MULLE_UTF_LIBRARY}")
   #
   # The order looks ascending, but due to the way this file is read
   # it ends up being descending, which is what we need.
   #
   if( MULLE_UTF_LIBRARY)
      #
      # Add MULLE_UTF_LIBRARY to DEPENDENCY_LIBRARIES list.
      # Disable with: `mulle-sourcetree mark mulle-utf no-cmake-add`
      #
      set( DEPENDENCY_LIBRARIES
         ${DEPENDENCY_LIBRARIES}
         ${MULLE_UTF_LIBRARY}
         CACHE INTERNAL "need to cache this"
      )
      #
      # Inherit information from dependency.
      # Encompasses: no-cmake-searchpath,no-cmake-dependency,no-cmake-loader
      # Disable with: `mulle-sourcetree mark mulle-utf no-cmake-inherit`
      #
      # temporarily expand CMAKE_MODULE_PATH
      get_filename_component( _TMP_MULLE_UTF_ROOT "${MULLE_UTF_LIBRARY}" DIRECTORY)
      get_filename_component( _TMP_MULLE_UTF_ROOT "${_TMP_MULLE_UTF_ROOT}" DIRECTORY)
      #
      #
      # Search for "DependenciesAndLibraries.cmake" to include.
      # Disable with: `mulle-sourcetree mark mulle-utf no-cmake-dependency`
      #
      foreach( _TMP_MULLE_UTF_NAME "mulle-utf")
         set( _TMP_MULLE_UTF_DIR "${_TMP_MULLE_UTF_ROOT}/include/${_TMP_MULLE_UTF_NAME}/cmake")
         # use explicit path to avoid "surprises"
         if( EXISTS "${_TMP_MULLE_UTF_DIR}/DependenciesAndLibraries.cmake")
            unset( MULLE_UTF_DEFINITIONS)
            list( INSERT CMAKE_MODULE_PATH 0 "${_TMP_MULLE_UTF_DIR}")
            # we only want top level INHERIT_OBJC_LOADERS, so disable them
            if( NOT NO_INHERIT_OBJC_LOADERS)
               set( NO_INHERIT_OBJC_LOADERS OFF)
            endif()
            list( APPEND _TMP_INHERIT_OBJC_LOADERS ${NO_INHERIT_OBJC_LOADERS})
            set( NO_INHERIT_OBJC_LOADERS ON)
            #
            include( "${_TMP_MULLE_UTF_DIR}/DependenciesAndLibraries.cmake")
            #
            list( GET _TMP_INHERIT_OBJC_LOADERS -1 NO_INHERIT_OBJC_LOADERS)
            list( REMOVE_AT _TMP_INHERIT_OBJC_LOADERS -1)
            #
            list( REMOVE_ITEM CMAKE_MODULE_PATH "${_TMP_MULLE_UTF_DIR}")
            set( INHERITED_DEFINITIONS
               ${INHERITED_DEFINITIONS}
               ${MULLE_UTF_DEFINITIONS}
               CACHE INTERNAL "need to cache this"
            )
            break()
         else()
            message( STATUS "${_TMP_MULLE_UTF_DIR}/DependenciesAndLibraries.cmake not found")
         endif()
      endforeach()
   else()
      # Disable with: `mulle-sourcetree mark mulle-utf no-require-link`
      message( FATAL_ERROR "MULLE_UTF_LIBRARY was not found")
   endif()
endif()


#
# Generated from sourcetree: 58C8E975-D4F6-42B5-AD5D-C7A59437088E;mulle-vararg;no-all-load,no-cmake-inherit,no-import,no-singlephase;
# Disable with : `mulle-sourcetree mark mulle-vararg no-link`
# Disable for this platform: `mulle-sourcetree mark mulle-vararg no-cmake-platform-linux`
#
if( NOT MULLE_VARARG_LIBRARY)
   find_library( MULLE_VARARG_LIBRARY NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-vararg${CMAKE_STATIC_LIBRARY_SUFFIX} mulle-vararg NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
   message( STATUS "MULLE_VARARG_LIBRARY is ${MULLE_VARARG_LIBRARY}")
   #
   # The order looks ascending, but due to the way this file is read
   # it ends up being descending, which is what we need.
   #
   if( MULLE_VARARG_LIBRARY)
      #
      # Add MULLE_VARARG_LIBRARY to DEPENDENCY_LIBRARIES list.
      # Disable with: `mulle-sourcetree mark mulle-vararg no-cmake-add`
      #
      set( DEPENDENCY_LIBRARIES
         ${DEPENDENCY_LIBRARIES}
         ${MULLE_VARARG_LIBRARY}
         CACHE INTERNAL "need to cache this"
      )
      # intentionally left blank
   else()
      # Disable with: `mulle-sourcetree mark mulle-vararg no-require-link`
      message( FATAL_ERROR "MULLE_VARARG_LIBRARY was not found")
   endif()
endif()


#
# Generated from sourcetree: 477069F3-D58B-4765-B971-147E63891606;mulle-thread;no-all-load,no-cmake-searchpath,no-import,no-singlephase;
# Disable with : `mulle-sourcetree mark mulle-thread no-link`
# Disable for this platform: `mulle-sourcetree mark mulle-thread no-cmake-platform-linux`
#
if( NOT MULLE_THREAD_LIBRARY)
   find_library( MULLE_THREAD_LIBRARY NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-thread${CMAKE_STATIC_LIBRARY_SUFFIX} mulle-thread NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
   message( STATUS "MULLE_THREAD_LIBRARY is ${MULLE_THREAD_LIBRARY}")
   #
   # The order looks ascending, but due to the way this file is read
   # it ends up being descending, which is what we need.
   #
   if( MULLE_THREAD_LIBRARY)
      #
      # Add MULLE_THREAD_LIBRARY to DEPENDENCY_LIBRARIES list.
      # Disable with: `mulle-sourcetree mark mulle-thread no-cmake-add`
      #
      set( DEPENDENCY_LIBRARIES
         ${DEPENDENCY_LIBRARIES}
         ${MULLE_THREAD_LIBRARY}
         CACHE INTERNAL "need to cache this"
      )
      #
      # Inherit information from dependency.
      # Encompasses: no-cmake-searchpath,no-cmake-dependency,no-cmake-loader
      # Disable with: `mulle-sourcetree mark mulle-thread no-cmake-inherit`
      #
      # temporarily expand CMAKE_MODULE_PATH
      get_filename_component( _TMP_MULLE_THREAD_ROOT "${MULLE_THREAD_LIBRARY}" DIRECTORY)
      get_filename_component( _TMP_MULLE_THREAD_ROOT "${_TMP_MULLE_THREAD_ROOT}" DIRECTORY)
      #
      #
      # Search for "DependenciesAndLibraries.cmake" to include.
      # Disable with: `mulle-sourcetree mark mulle-thread no-cmake-dependency`
      #
      foreach( _TMP_MULLE_THREAD_NAME "mulle-thread")
         set( _TMP_MULLE_THREAD_DIR "${_TMP_MULLE_THREAD_ROOT}/include/${_TMP_MULLE_THREAD_NAME}/cmake")
         # use explicit path to avoid "surprises"
         if( EXISTS "${_TMP_MULLE_THREAD_DIR}/DependenciesAndLibraries.cmake")
            unset( MULLE_THREAD_DEFINITIONS)
            list( INSERT CMAKE_MODULE_PATH 0 "${_TMP_MULLE_THREAD_DIR}")
            # we only want top level INHERIT_OBJC_LOADERS, so disable them
            if( NOT NO_INHERIT_OBJC_LOADERS)
               set( NO_INHERIT_OBJC_LOADERS OFF)
            endif()
            list( APPEND _TMP_INHERIT_OBJC_LOADERS ${NO_INHERIT_OBJC_LOADERS})
            set( NO_INHERIT_OBJC_LOADERS ON)
            #
            include( "${_TMP_MULLE_THREAD_DIR}/DependenciesAndLibraries.cmake")
            #
            list( GET _TMP_INHERIT_OBJC_LOADERS -1 NO_INHERIT_OBJC_LOADERS)
            list( REMOVE_AT _TMP_INHERIT_OBJC_LOADERS -1)
            #
            list( REMOVE_ITEM CMAKE_MODULE_PATH "${_TMP_MULLE_THREAD_DIR}")
            set( INHERITED_DEFINITIONS
               ${INHERITED_DEFINITIONS}
               ${MULLE_THREAD_DEFINITIONS}
               CACHE INTERNAL "need to cache this"
            )
            break()
         else()
            message( STATUS "${_TMP_MULLE_THREAD_DIR}/DependenciesAndLibraries.cmake not found")
         endif()
      endforeach()
   else()
      # Disable with: `mulle-sourcetree mark mulle-thread no-require-link`
      message( FATAL_ERROR "MULLE_THREAD_LIBRARY was not found")
   endif()
endif()
