//
//  mulle-sprintf-pointer.c
//  mulle-sprintf
//
//  Copyright (c) 2018 Nat! - Mulle kybernetiK.
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
#include "mulle-sprintf-pointer.h"

#include "mulle-sprintf-integer.h"
#include "mulle-sprintf.h"

#include <stdint.h>



//static int   object_conversion( struct mulle_sprintf_formatconversioninfo *info,
//                                struct mulle_buffer *buffer,
//                                struct mulle_sprintf_argumentarray *arguments,
//                                int argc)
//{
//   union mulle_sprintf_argumentvalue  v;
//
//   v = arguments->values[ argc];
//   if( ! info->memory.width_found && ! info->memory.precision_found)
//      info = NULL;
//   return( (*struct mulle_bufferObject_conversion)( info, buffer, v.obj));
//}


static int   set_pointer_prefix( char *s, int value_is_zero, int length, int precision)
{
   MULLE_C_UNUSED( value_is_zero);
   MULLE_C_UNUSED( precision);

   if( length)
   {
      s[ 0] = '0';
      s[ 1] = 'x';
      return( 2);
   }
   return( 0);
}


static int  _mulle_sprintf_pointer_conversion( struct mulle_buffer *buffer,
                                               struct mulle_sprintf_formatconversioninfo *info,
                                               struct mulle_sprintf_argumentarray *arguments,
                                               int argc)
{
   union mulle_sprintf_argumentvalue  v;
   uintptr_t                          p;
   char                               tmp[ sizeof( uintptr_t) * 2 + 1];
   char                               *s;
   char                               c;
   int                                length;

   v = arguments->values[ argc];

   // %p has no defined # or 0 semantics; match glibc's "(nil)" for NULL
   // (glibc pads with spaces, never '0', regardless of the 0 flag)
   if( ! v.pv)
   {
      static char  nil[] = "(nil)";
      int          zero  = info->memory.zero_found;

      info->memory.zero_found = 0;
      _mulle_sprintf_justified( buffer, info,
                                nil,
                                5,
                                NULL,
                                0,
                                0,
                                0);
      info->memory.zero_found = zero;
      return( 0);
   }

   // well-defined conversion: void * -> uintptr_t
   p = (uintptr_t) v.pv;

   s = &tmp[ sizeof( tmp)];
   do
   {
      c     = p & 0xF;
      *--s  = (c >= 10) ? ('a' - 10 + c) : '0' + c;
      p  >>= 4;
   }
   while( p);

   length = &tmp[ sizeof( tmp)] - s;

   // make the shared justification machinery emit the 0x prefix
   info->memory.hash_found = 1;
   _mulle_sprintf_justified_and_prefixed( buffer, info,
                                          s,
                                          length,
                                          0,
                                          0,
                                          set_pointer_prefix);
   return( 0);
}



static mulle_sprintf_argumenttype_t
   mulle_sprintf_get_pointer_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   assert( info->modifier[ 0] == '\0');

   MULLE_C_UNUSED( info);

   return( mulle_sprintf_void_pointer_argumenttype);
}


static struct mulle_sprintf_function     mulle_sprintf_pointer_functions =
{
   mulle_sprintf_get_pointer_argumenttype,
   _mulle_sprintf_pointer_conversion
};



void  mulle_sprintf_register_pointer_functions( struct mulle_sprintf_conversion *tables)
{
   mulle_sprintf_register_functions( tables, &mulle_sprintf_pointer_functions, 'p');
}


// MULLE_C_CONSTRUCTOR( mulle_sprintf_register_default_pointer_functions)
// static void  mulle_sprintf_register_default_pointer_functions()
// {
//   mulle_sprintf_register_pointer_functions( mulle_sprintf_get_defaultconversion());
// }


