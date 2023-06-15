//
//  mulle_sprintf_return.c
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
#include "mulle-sprintf-return.h"

#include "mulle-sprintf.h"


//
// in the spec, it's declared as a signed return value (but what for ?)
// might be a subtle bug
//
static int   _mulle_sprintf_return_conversion( struct mulle_buffer *buffer,
                                               struct mulle_sprintf_formatconversioninfo *info,
                                               struct mulle_sprintf_argumentarray *arguments,
                                               int argc)
{
   union mulle_sprintf_argumentvalue  v;
   mulle_sprintf_argumenttype_t   t;
   size_t          len;
   int             before;

   assert( buffer);
   assert( info);
   assert( arguments);

   before = (int) (intptr_t) info->mystery;

   v = arguments->values[ argc];
   t = arguments->types[ argc];

   //
   // this will give too many bytes if the initial flex buffer
   // was not empty. But that's not really a bug, it's a feature
   // (if we _really_ really need this, pass in "before" from caller
   //
   len = mulle_buffer_get_length( buffer) - before;

   if( t == mulle_sprintf_unsigned_int_pointer_argumenttype)
   {
      *v.pI = (unsigned int) len;
      return( 0);
   }
   if( t == mulle_sprintf_unsigned_long_pointer_argumenttype)
   {
      *v.pL = (unsigned long) len;
      return( 0);
   }
   if( t == mulle_sprintf_unsigned_long_long_pointer_argumenttype)
   {
      *v.pLL = (unsigned long long) len;
      return( 0);
   }
   if( t == mulle_sprintf_unsigned_short_argumenttype)
   {
      *v.pS = (unsigned short) len;
      return( 0);
   }
   if( t == mulle_sprintf_unsigned_char_argumenttype)
   {
      *v.pC = (unsigned char) len;
      return( 0);
   }
   if( t == mulle_sprintf_uintmax_t_pointer_argumenttype)
   {
      *v.pImt = (uintmax_t) len;
      return( 0);
   }
   if( t == mulle_sprintf_unsigned_ptrdiff_t_pointer_argumenttype)
   {
      *v.pDif = (unsigned MULLE_SPRINTF_PTRDIFF_TYPE) len;
      return( 0);
   }
   if( t == mulle_sprintf_size_t_pointer_argumenttype)
   {
      *v.pSt = (size_t) len;
      return( 0);
   }
   if( t == mulle_sprintf_uint64_t_pointer_argumenttype)
   {
      *v.pQt = (uint64_t) len;
      return( 0);
   }
   return( -1);
}


static mulle_sprintf_argumenttype_t  _mulle_sprintf_get_return_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   switch( info->modifier[ 0])
   {
   case 'h' :
      if( info->modifier[ 1] == 'h')
      {
         assert( info->modifier[ 2] == '\0');
         return( mulle_sprintf_unsigned_char_pointer_argumenttype);
      }
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_unsigned_short_pointer_argumenttype);

   case 'l' :
      if( info->modifier[ 1] == 'l')
      {
         assert( info->modifier[ 2] == '\0');
         return( mulle_sprintf_unsigned_long_long_pointer_argumenttype);
      }
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_unsigned_long_pointer_argumenttype);

   case 'j' :
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_size_t_pointer_argumenttype);

   case 't' :
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_unsigned_ptrdiff_t_pointer_argumenttype);

   case 'z' :
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_uintmax_t_pointer_argumenttype);

   case 'q' :
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_uint64_t_pointer_argumenttype);
   }
   assert( info->modifier[ 0] == '\0');
   return( mulle_sprintf_unsigned_int_pointer_argumenttype);
}


static struct mulle_sprintf_function     mulle_sprintf_return_functions =
{
   _mulle_sprintf_get_return_argumenttype,
   _mulle_sprintf_return_conversion
};



void  mulle_sprintf_register_return_functions( struct mulle_sprintf_conversion *tables)
{
   mulle_sprintf_register_functions( tables, &mulle_sprintf_return_functions, 'n');
}


// MULLE_C_CONSTRUCTOR( mulle_sprintf_register_default_return_functions)
// static void  mulle_sprintf_register_default_return_functions()
// {
//    mulle_sprintf_register_return_functions( mulle_sprintf_get_defaultconversion());
// }

