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

#include "dependencies.h"

#include "mulle-sprintf-function.h"


#define MULLE_SPRINTF_VERSION  ((1 << 20) | (0 << 8) | 0)


#ifndef MULLE_SPRINTF_EXTERN_GLOBAL
# define MULLE_SPRINTF_EXTERN_GLOBAL    MULLE_C_EXTERN_GLOBAL
#endif



struct mulle_buffer;
struct mulle_sprintf_conversion;

// will not append '\0' !
int   mulle_sprintf( struct mulle_buffer *buffer,
                     char *format,
                     ...);

int   mulle_vsprintf( struct mulle_buffer *buffer,
                      char *format,
                      va_list va);

int   _mulle_vsprintf( struct mulle_buffer *buffer,
                       char *format,
                       va_list va,
                       struct mulle_sprintf_conversion *table);

int   mulle_mvsprintf( struct mulle_buffer *buffer,
                       char *format,
                       mulle_vararg_list va);

int   _mulle_mvsprintf( struct mulle_buffer *buffer,
                        char *format,
                        mulle_vararg_list arguments,
                        struct mulle_sprintf_conversion *table);

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


static inline struct mulle_sprintf_conversion   *mulle_sprintf_get_defaultconversion( void)
{
   return( &mulle_sprintf_get_config()->defaultconversion);
}



static inline void    mulle_sprintf_free_storage( void)
{
   (*mulle_sprintf_get_config()->free_storage)();
}


#if MULLE_ALLOCATOR_VERSION < ((1 << 20) | (5 << 8) | 0)
# error "mulle_allocator is too old"
#endif
#if MULLE_BUFFER_VERSION < ((0 << 20) | (4 << 8) | 0)
# error "mulle_buffer is too old"
#endif

#include "mulle-sprintf-character.h"
#include "mulle-sprintf-escape.h"
#include "mulle-sprintf-fp.h"
#include "mulle-sprintf-integer.h"
#include "mulle-sprintf-pointer.h"
#include "mulle-sprintf-return.h"
#include "mulle-sprintf-string.h"

#endif
