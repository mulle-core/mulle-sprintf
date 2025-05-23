# This file will be regenerated by `mulle-match-to-cmake` via
# `mulle-sde reflect` and any edits will be lost.
#
# This file will be included by cmake/share/Headers.cmake
#
if( MULLE_TRACE_INCLUDE)
   MESSAGE( STATUS "# Include \"${CMAKE_CURRENT_LIST_FILE}\"" )
endif()

#
# contents are derived from the file locations

set( INCLUDE_DIRS
src/reflect
src/generic
src/functions
src
)

#
# contents selected with patternfile ??-header--private-generated-headers
#
set( PRIVATE_GENERATED_HEADERS
src/reflect/_mulle-sprintf-include-private.h
)

#
# contents selected with patternfile ??-header--private-generic-headers
#
set( PRIVATE_GENERIC_HEADERS
src/generic/include-private.h
)

#
# contents selected with patternfile ??-header--public-generated-headers
#
set( PUBLIC_GENERATED_HEADERS
src/reflect/_mulle-sprintf-include.h
src/reflect/_mulle-sprintf-versioncheck.h
)

#
# contents selected with patternfile ??-header--public-generic-headers
#
set( PUBLIC_GENERIC_HEADERS
src/generic/include.h
)

#
# contents selected with patternfile ??-header--public-headers
#
set( PUBLIC_HEADERS
src/functions/mulle-sprintf-character.h
src/functions/mulle-sprintf-escape.h
src/functions/mulle-sprintf-fp.h
src/functions/mulle-sprintf-integer.h
src/functions/mulle-sprintf-pointer.h
src/functions/mulle-sprintf-return.h
src/functions/mulle-sprintf-string.h
src/mulle-sprintf-function.h
src/mulle-sprintf.h
)

