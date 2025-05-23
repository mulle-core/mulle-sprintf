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
# Disable for this platform: `mulle-sourcetree mark mulle-buffer no-cmake-platform-${MULLE_UNAME}`
# Disable for a sdk: `mulle-sourcetree mark mulle-buffer no-cmake-sdk-<name>`
#
if( COLLECT_DEPENDENCY_LIBRARIES_AS_NAMES)
   list( APPEND DEPENDENCY_LIBRARIES "mulle-buffer")
else()
   if( NOT MULLE__BUFFER_LIBRARY)
      find_library( MULLE__BUFFER_LIBRARY NAMES
         ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-buffer${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
         ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-buffer${CMAKE_STATIC_LIBRARY_SUFFIX}
         mulle-buffer
         NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH
      )
      if( NOT MULLE__BUFFER_LIBRARY AND NOT DEPENDENCY_IGNORE_SYSTEM_LIBARIES)
         find_library( MULLE__BUFFER_LIBRARY NAMES
            ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-buffer${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-buffer${CMAKE_STATIC_LIBRARY_SUFFIX}
            mulle-buffer
         )
      endif()
      message( STATUS "MULLE__BUFFER_LIBRARY is ${MULLE__BUFFER_LIBRARY}")
      #
      # The order looks ascending, but due to the way this file is read
      # it ends up being descending, which is what we need.
      #
      if( MULLE__BUFFER_LIBRARY)
         #
         # Add MULLE__BUFFER_LIBRARY to DEPENDENCY_LIBRARIES list.
         # Disable with: `mulle-sourcetree mark mulle-buffer no-cmake-add`
         #
         list( APPEND DEPENDENCY_LIBRARIES ${MULLE__BUFFER_LIBRARY})
         #
         # Inherit information from dependency.
         # Encompasses: no-cmake-searchpath,no-cmake-dependency,no-cmake-loader
         # Disable with: `mulle-sourcetree mark mulle-buffer no-cmake-inherit`
         #
         # temporarily expand CMAKE_MODULE_PATH
         get_filename_component( _TMP_MULLE__BUFFER_ROOT "${MULLE__BUFFER_LIBRARY}" DIRECTORY)
         get_filename_component( _TMP_MULLE__BUFFER_ROOT "${_TMP_MULLE__BUFFER_ROOT}" DIRECTORY)
         #
         #
         # Search for "Definitions.cmake" and "DependenciesAndLibraries.cmake" to include.
         # Disable with: `mulle-sourcetree mark mulle-buffer no-cmake-dependency`
         #
         foreach( _TMP_MULLE__BUFFER_NAME "mulle-buffer")
            set( _TMP_MULLE__BUFFER_DIR "${_TMP_MULLE__BUFFER_ROOT}/include/${_TMP_MULLE__BUFFER_NAME}/cmake")
            # use explicit path to avoid "surprises"
            if( IS_DIRECTORY "${_TMP_MULLE__BUFFER_DIR}")
               list( INSERT CMAKE_MODULE_PATH 0 "${_TMP_MULLE__BUFFER_DIR}")
               # we only want top level INHERIT_OBJC_LOADERS, so disable them
               if( NOT NO_INHERIT_OBJC_LOADERS)
                  set( NO_INHERIT_OBJC_LOADERS OFF)
               endif()
               list( APPEND _TMP_INHERIT_OBJC_LOADERS ${NO_INHERIT_OBJC_LOADERS})
               set( NO_INHERIT_OBJC_LOADERS ON)
               #
               include( "${_TMP_MULLE__BUFFER_DIR}/DependenciesAndLibraries.cmake" OPTIONAL)
               #
               list( GET _TMP_INHERIT_OBJC_LOADERS -1 NO_INHERIT_OBJC_LOADERS)
               list( REMOVE_AT _TMP_INHERIT_OBJC_LOADERS -1)
               list( REMOVE_ITEM CMAKE_MODULE_PATH "${_TMP_MULLE__BUFFER_DIR}")
               #
               unset( MULLE__BUFFER_DEFINITIONS)
               include( "${_TMP_MULLE__BUFFER_DIR}/Definitions.cmake" OPTIONAL)
               list( APPEND INHERITED_DEFINITIONS ${MULLE__BUFFER_DEFINITIONS})
               break()
            else()
               message( STATUS "${_TMP_MULLE__BUFFER_DIR} not found")
            endif()
         endforeach()
      else()
         # Disable with: `mulle-sourcetree mark mulle-buffer no-require-link`
         message( SEND_ERROR "MULLE__BUFFER_LIBRARY was not found")
      endif()
   endif()
endif()


#
# Generated from sourcetree: CD4D015D-20FB-456F-B8CD-43CEEB5DCA19;mulle-utf;no-all-load,no-cmake-searchpath,no-import,no-singlephase;
# Disable with : `mulle-sourcetree mark mulle-utf no-link`
# Disable for this platform: `mulle-sourcetree mark mulle-utf no-cmake-platform-${MULLE_UNAME}`
# Disable for a sdk: `mulle-sourcetree mark mulle-utf no-cmake-sdk-<name>`
#
if( COLLECT_DEPENDENCY_LIBRARIES_AS_NAMES)
   list( APPEND DEPENDENCY_LIBRARIES "mulle-utf")
else()
   if( NOT MULLE__UTF_LIBRARY)
      find_library( MULLE__UTF_LIBRARY NAMES
         ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-utf${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
         ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-utf${CMAKE_STATIC_LIBRARY_SUFFIX}
         mulle-utf
         NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH
      )
      if( NOT MULLE__UTF_LIBRARY AND NOT DEPENDENCY_IGNORE_SYSTEM_LIBARIES)
         find_library( MULLE__UTF_LIBRARY NAMES
            ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-utf${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-utf${CMAKE_STATIC_LIBRARY_SUFFIX}
            mulle-utf
         )
      endif()
      message( STATUS "MULLE__UTF_LIBRARY is ${MULLE__UTF_LIBRARY}")
      #
      # The order looks ascending, but due to the way this file is read
      # it ends up being descending, which is what we need.
      #
      if( MULLE__UTF_LIBRARY)
         #
         # Add MULLE__UTF_LIBRARY to DEPENDENCY_LIBRARIES list.
         # Disable with: `mulle-sourcetree mark mulle-utf no-cmake-add`
         #
         list( APPEND DEPENDENCY_LIBRARIES ${MULLE__UTF_LIBRARY})
         #
         # Inherit information from dependency.
         # Encompasses: no-cmake-searchpath,no-cmake-dependency,no-cmake-loader
         # Disable with: `mulle-sourcetree mark mulle-utf no-cmake-inherit`
         #
         # temporarily expand CMAKE_MODULE_PATH
         get_filename_component( _TMP_MULLE__UTF_ROOT "${MULLE__UTF_LIBRARY}" DIRECTORY)
         get_filename_component( _TMP_MULLE__UTF_ROOT "${_TMP_MULLE__UTF_ROOT}" DIRECTORY)
         #
         #
         # Search for "Definitions.cmake" and "DependenciesAndLibraries.cmake" to include.
         # Disable with: `mulle-sourcetree mark mulle-utf no-cmake-dependency`
         #
         foreach( _TMP_MULLE__UTF_NAME "mulle-utf")
            set( _TMP_MULLE__UTF_DIR "${_TMP_MULLE__UTF_ROOT}/include/${_TMP_MULLE__UTF_NAME}/cmake")
            # use explicit path to avoid "surprises"
            if( IS_DIRECTORY "${_TMP_MULLE__UTF_DIR}")
               list( INSERT CMAKE_MODULE_PATH 0 "${_TMP_MULLE__UTF_DIR}")
               # we only want top level INHERIT_OBJC_LOADERS, so disable them
               if( NOT NO_INHERIT_OBJC_LOADERS)
                  set( NO_INHERIT_OBJC_LOADERS OFF)
               endif()
               list( APPEND _TMP_INHERIT_OBJC_LOADERS ${NO_INHERIT_OBJC_LOADERS})
               set( NO_INHERIT_OBJC_LOADERS ON)
               #
               include( "${_TMP_MULLE__UTF_DIR}/DependenciesAndLibraries.cmake" OPTIONAL)
               #
               list( GET _TMP_INHERIT_OBJC_LOADERS -1 NO_INHERIT_OBJC_LOADERS)
               list( REMOVE_AT _TMP_INHERIT_OBJC_LOADERS -1)
               list( REMOVE_ITEM CMAKE_MODULE_PATH "${_TMP_MULLE__UTF_DIR}")
               #
               unset( MULLE__UTF_DEFINITIONS)
               include( "${_TMP_MULLE__UTF_DIR}/Definitions.cmake" OPTIONAL)
               list( APPEND INHERITED_DEFINITIONS ${MULLE__UTF_DEFINITIONS})
               break()
            else()
               message( STATUS "${_TMP_MULLE__UTF_DIR} not found")
            endif()
         endforeach()
      else()
         # Disable with: `mulle-sourcetree mark mulle-utf no-require-link`
         message( SEND_ERROR "MULLE__UTF_LIBRARY was not found")
      endif()
   endif()
endif()


#
# Generated from sourcetree: 58C8E975-D4F6-42B5-AD5D-C7A59437088E;mulle-vararg;no-all-load,no-cmake-inherit,no-import,no-singlephase;
# Disable with : `mulle-sourcetree mark mulle-vararg no-link`
# Disable for this platform: `mulle-sourcetree mark mulle-vararg no-cmake-platform-${MULLE_UNAME}`
# Disable for a sdk: `mulle-sourcetree mark mulle-vararg no-cmake-sdk-<name>`
#
if( COLLECT_DEPENDENCY_LIBRARIES_AS_NAMES)
   list( APPEND DEPENDENCY_LIBRARIES "mulle-vararg")
else()
   if( NOT MULLE__VARARG_LIBRARY)
      find_library( MULLE__VARARG_LIBRARY NAMES
         ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-vararg${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
         ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-vararg${CMAKE_STATIC_LIBRARY_SUFFIX}
         mulle-vararg
         NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH
      )
      if( NOT MULLE__VARARG_LIBRARY AND NOT DEPENDENCY_IGNORE_SYSTEM_LIBARIES)
         find_library( MULLE__VARARG_LIBRARY NAMES
            ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-vararg${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-vararg${CMAKE_STATIC_LIBRARY_SUFFIX}
            mulle-vararg
         )
      endif()
      message( STATUS "MULLE__VARARG_LIBRARY is ${MULLE__VARARG_LIBRARY}")
      #
      # The order looks ascending, but due to the way this file is read
      # it ends up being descending, which is what we need.
      #
      if( MULLE__VARARG_LIBRARY)
         #
         # Add MULLE__VARARG_LIBRARY to DEPENDENCY_LIBRARIES list.
         # Disable with: `mulle-sourcetree mark mulle-vararg no-cmake-add`
         #
         list( APPEND DEPENDENCY_LIBRARIES ${MULLE__VARARG_LIBRARY})
         # intentionally left blank
      else()
         # Disable with: `mulle-sourcetree mark mulle-vararg no-require-link`
         message( SEND_ERROR "MULLE__VARARG_LIBRARY was not found")
      endif()
   endif()
endif()


#
# Generated from sourcetree: 477069F3-D58B-4765-B971-147E63891606;mulle-thread;no-all-load,no-cmake-searchpath,no-import,no-singlephase;
# Disable with : `mulle-sourcetree mark mulle-thread no-link`
# Disable for this platform: `mulle-sourcetree mark mulle-thread no-cmake-platform-${MULLE_UNAME}`
# Disable for a sdk: `mulle-sourcetree mark mulle-thread no-cmake-sdk-<name>`
#
if( COLLECT_DEPENDENCY_LIBRARIES_AS_NAMES)
   list( APPEND DEPENDENCY_LIBRARIES "mulle-thread")
else()
   if( NOT MULLE__THREAD_LIBRARY)
      find_library( MULLE__THREAD_LIBRARY NAMES
         ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-thread${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
         ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-thread${CMAKE_STATIC_LIBRARY_SUFFIX}
         mulle-thread
         NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH
      )
      if( NOT MULLE__THREAD_LIBRARY AND NOT DEPENDENCY_IGNORE_SYSTEM_LIBARIES)
         find_library( MULLE__THREAD_LIBRARY NAMES
            ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-thread${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-thread${CMAKE_STATIC_LIBRARY_SUFFIX}
            mulle-thread
         )
      endif()
      message( STATUS "MULLE__THREAD_LIBRARY is ${MULLE__THREAD_LIBRARY}")
      #
      # The order looks ascending, but due to the way this file is read
      # it ends up being descending, which is what we need.
      #
      if( MULLE__THREAD_LIBRARY)
         #
         # Add MULLE__THREAD_LIBRARY to DEPENDENCY_LIBRARIES list.
         # Disable with: `mulle-sourcetree mark mulle-thread no-cmake-add`
         #
         list( APPEND DEPENDENCY_LIBRARIES ${MULLE__THREAD_LIBRARY})
         #
         # Inherit information from dependency.
         # Encompasses: no-cmake-searchpath,no-cmake-dependency,no-cmake-loader
         # Disable with: `mulle-sourcetree mark mulle-thread no-cmake-inherit`
         #
         # temporarily expand CMAKE_MODULE_PATH
         get_filename_component( _TMP_MULLE__THREAD_ROOT "${MULLE__THREAD_LIBRARY}" DIRECTORY)
         get_filename_component( _TMP_MULLE__THREAD_ROOT "${_TMP_MULLE__THREAD_ROOT}" DIRECTORY)
         #
         #
         # Search for "Definitions.cmake" and "DependenciesAndLibraries.cmake" to include.
         # Disable with: `mulle-sourcetree mark mulle-thread no-cmake-dependency`
         #
         foreach( _TMP_MULLE__THREAD_NAME "mulle-thread")
            set( _TMP_MULLE__THREAD_DIR "${_TMP_MULLE__THREAD_ROOT}/include/${_TMP_MULLE__THREAD_NAME}/cmake")
            # use explicit path to avoid "surprises"
            if( IS_DIRECTORY "${_TMP_MULLE__THREAD_DIR}")
               list( INSERT CMAKE_MODULE_PATH 0 "${_TMP_MULLE__THREAD_DIR}")
               # we only want top level INHERIT_OBJC_LOADERS, so disable them
               if( NOT NO_INHERIT_OBJC_LOADERS)
                  set( NO_INHERIT_OBJC_LOADERS OFF)
               endif()
               list( APPEND _TMP_INHERIT_OBJC_LOADERS ${NO_INHERIT_OBJC_LOADERS})
               set( NO_INHERIT_OBJC_LOADERS ON)
               #
               include( "${_TMP_MULLE__THREAD_DIR}/DependenciesAndLibraries.cmake" OPTIONAL)
               #
               list( GET _TMP_INHERIT_OBJC_LOADERS -1 NO_INHERIT_OBJC_LOADERS)
               list( REMOVE_AT _TMP_INHERIT_OBJC_LOADERS -1)
               list( REMOVE_ITEM CMAKE_MODULE_PATH "${_TMP_MULLE__THREAD_DIR}")
               #
               unset( MULLE__THREAD_DEFINITIONS)
               include( "${_TMP_MULLE__THREAD_DIR}/Definitions.cmake" OPTIONAL)
               list( APPEND INHERITED_DEFINITIONS ${MULLE__THREAD_DEFINITIONS})
               break()
            else()
               message( STATUS "${_TMP_MULLE__THREAD_DIR} not found")
            endif()
         endforeach()
      else()
         # Disable with: `mulle-sourcetree mark mulle-thread no-require-link`
         message( SEND_ERROR "MULLE__THREAD_LIBRARY was not found")
      endif()
   endif()
endif()
