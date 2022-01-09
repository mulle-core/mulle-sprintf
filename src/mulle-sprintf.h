//
//  mulle_sprintf.h
//  mulle-sprintf
//
//  Created by Nat!
//  Copyright (c) 2011 Nat! - Mulle kybernetiK.
//  Copyright (c) 2011 Codeon GmbH.
//  All rights reserved.
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  Neither the name of Mulle kybernetiK nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//

#ifndef mulle_sprintf_h__
#define mulle_sprintf_h__

#include "include.h"

#include "mulle-sprintf-function.h"


#define MULLE_SPRINTF_VERSION  ((2 << 20) | (0 << 8) | 0)


#ifndef MULLE_SPRINTF_EXTERN_GLOBAL
# define MULLE_SPRINTF_EXTERN_GLOBAL    MULLE_C_EXTERN_GLOBAL
#endif



struct mulle_buffer;
struct mulle_sprintf_conversion;

// will not append '\0' !
MULLE_SPRINTF_EXTERN_GLOBAL
int   mulle_buffer_sprintf( struct mulle_buffer *buffer,
                            char *format,
                            ...);

MULLE_SPRINTF_EXTERN_GLOBAL
int   mulle_buffer_vsprintf( struct mulle_buffer *buffer,
                             char *format,
                             va_list va);

MULLE_SPRINTF_EXTERN_GLOBAL
int   _mulle_buffer_vsprintf( struct mulle_buffer *buffer,
                              char *format,
                              va_list va,
                              struct mulle_sprintf_conversion *table);

MULLE_SPRINTF_EXTERN_GLOBAL
int   mulle_buffer_mvsprintf( struct mulle_buffer *buffer,
                              char *format,
                              mulle_vararg_list va);

MULLE_SPRINTF_EXTERN_GLOBAL
int   _mulle_buffer_mvsprintf( struct mulle_buffer *buffer,
                               char *format,
                               mulle_vararg_list arguments,
                               struct mulle_sprintf_conversion *table);


//
// These functions provide a nice mapping from C stdio to
// mulle sprintf. They use a mulle_buffer internally
// but they return -1 on error, not the size that needs to be printed.
// These functions append a '0'.
//
MULLE_SPRINTF_EXTERN_GLOBAL
int   mulle_snprintf( char *buf, size_t size, char *format, ...);

MULLE_SPRINTF_EXTERN_GLOBAL
int   mulle_vsnprintf( char *buf, size_t size, char *format, va_list va);

MULLE_SPRINTF_EXTERN_GLOBAL
int   mulle_mvsnprintf( char *buf, size_t size, char *format, mulle_vararg_list arguments);

//
// Or use these unsafer sprintf versions,
// Use the buffer versions if you need flexibility.
//
// You shouldn't use them though. Use the buffer versions
MULLE_SPRINTF_EXTERN_GLOBAL
int   mulle_sprintf( char *buf, char *format, ...);


static inline int   mulle_vsprintf( char *buf, char *format, va_list va)
{
   return( mulle_vsnprintf( buf, INT_MAX, format, va));
}


static inline int   mulle_mvsprintf( char *buf, char *format, mulle_vararg_list arguments)
{
   return( mulle_mvsnprintf( buf, INT_MAX, format, arguments));
}


//
// here are also the asprintf variety, theses return a mulle_default_allocator
// allocated string in *strp. So you need to free it with mulle_free(!) not
// free. It used to mulle_stdlib_allocator, but using "free" to free it is then
// too strange for me. If you are replacing existing code, chances are high
// you want to use mulle_malloc also, and then free is odd.
//
MULLE_SPRINTF_EXTERN_GLOBAL
int   mulle_asprintf(char **strp, char *format, ...);

MULLE_SPRINTF_EXTERN_GLOBAL
int   mulle_vasprintf(char **strp, char *format, va_list ap);

MULLE_SPRINTF_EXTERN_GLOBAL
int   mulle_mvasprintf(char **strp, char *format, mulle_vararg_list arguments);


#pragma mark - manage "sprintf"

struct mulle_sprintf_config
{
   mulle_thread_tss_t               key;
   void                             *(*get_storage)( struct mulle_allocator *);
   void                             (*free_storage)(void);
   struct mulle_sprintf_conversion  defaultconversion;
};


static inline struct mulle_sprintf_config  *mulle_sprintf_get_config( void)
{
   MULLE_SPRINTF_EXTERN_GLOBAL struct mulle_sprintf_config   mulle_sprintf_config;

   return( &mulle_sprintf_config);
}


static inline struct mulle_sprintf_conversion   *
  mulle_sprintf_get_defaultconversion( void)
{
   return( &mulle_sprintf_get_config()->defaultconversion);
}


static inline void    mulle_sprintf_free_storage( void)
{
   (*mulle_sprintf_get_config()->free_storage)();
}

#include "mulle-sprintf-character.h"
#include "mulle-sprintf-escape.h"
#include "mulle-sprintf-fp.h"
#include "mulle-sprintf-integer.h"
#include "mulle-sprintf-pointer.h"
#include "mulle-sprintf-return.h"
#include "mulle-sprintf-string.h"

#ifdef __has_include
# if __has_include( "_mulle-sprintf-versioncheck.h")
#  include "_mulle-sprintf-versioncheck.h"
# endif
#endif

#endif
