//
//  mulle_sprintf_function.c
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
#pragma clang diagnostic ignored "-Wparentheses"

#include "mulle-sprintf-function.h"

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <wctype.h>
#include "include-private.h"
#include "mulle-sprintf.h"


#define SIZEOF( type)  sizeof( type) // < sizeof( int) ? sizeof( int) : sizeof( type)

unsigned char   mulle_sprintf_argumentsize[] =
{
   SIZEOF( int),              // mulle_sprintf_int_argumenttype
   SIZEOF( char),
   SIZEOF( char *),
   SIZEOF( double),

   SIZEOF( intmax_t),         // mulle_sprintf_intmax_t_argumenttype
   SIZEOF( long),
   SIZEOF( long double),
   SIZEOF( long long),

   SIZEOF( void *),           // mulle_sprintf_object_argumenttype
   SIZEOF( ptrdiff_t),
   SIZEOF( int64_t),
   SIZEOF( short),

   SIZEOF( size_t),           // mulle_sprintf_signed_size_t_argumenttype (unused)
   SIZEOF( size_t),
   SIZEOF( size_t *),
   SIZEOF( uint64_t),

   SIZEOF( uint64_t *),      // mulle_sprintf_uint64_t_pointer_argumenttype
   SIZEOF( uintmax_t),
   SIZEOF( uintmax_t *),
   SIZEOF( unsigned char),

   SIZEOF( unsigned char *), // mulle_sprintf_unsigned_char_pointer_argumenttype
   SIZEOF( unsigned int),
   SIZEOF( unsigned int *),
   SIZEOF( unsigned long),

   SIZEOF( unsigned long long), // mulle_sprintf_unsigned_long_long_argumenttype
   SIZEOF( unsigned long long *),
   SIZEOF( unsigned long *),
   SIZEOF( unsigned MULLE_SPRINTF_PTRDIFF_TYPE),

   SIZEOF( unsigned MULLE_SPRINTF_PTRDIFF_TYPE *), // mulle_sprintf_unsigned_ptrdiff_t_pointer_argumenttype
   SIZEOF( unsigned short),
   SIZEOF( unsigned short *),
   0,    // vector

   0,    // void                 // mulle_sprintf_void_argumenttype
   SIZEOF( void *),
   SIZEOF( wchar_t *),
   SIZEOF( uint16_t *),
   SIZEOF( uint32_t *),
   SIZEOF( wint_t),

   SIZEOF( struct _NSDecimal *)  // mulle_sprintf_NSDecimal_pointer_argumenttype
};


// needs to be done in  a loop here, because va_arg calls can't be separated
// across function calls (interestingly)
//
void  mulle_vsprintf_set_values( union mulle_sprintf_argumentvalue *p,
                                 mulle_sprintf_argumenttype_t  *type,
                                 unsigned int n,
                                 va_list va)
{
   union mulle_sprintf_argumentvalue   *sentinel;

   sentinel = &p[ n];
   p        = &p[ 1];
   type     = &type[ 1];

   while( p < sentinel)
   {
      switch( *type)
      {
      default                                                : abort();

      case mulle_sprintf_int_argumenttype                    : p->i    = va_arg( va, int); break;

      case mulle_sprintf_char_argumenttype                   : p->c    = (char) va_arg( va, int); break;  // promotion(!)
      case mulle_sprintf_char_pointer_argumenttype           : p->pc   = va_arg( va, char *); break;
      case mulle_sprintf_double_argumenttype                 : p->d    = va_arg( va, double); break;
      case mulle_sprintf_intmax_t_argumenttype               : p->imt  = va_arg( va, intmax_t); break;
      case mulle_sprintf_long_argumenttype                   : p->l    = va_arg( va, long); break;
      case mulle_sprintf_long_double_argumenttype            : p->ld   = va_arg( va, long double); break;
      case mulle_sprintf_long_long_argumenttype              : p->ll   = va_arg( va, long long); break;
      case mulle_sprintf_void_argumenttype                   : break;
      case mulle_sprintf_ptrdiff_t_argumenttype              : p->dif  = va_arg( va, ptrdiff_t); break;
      case mulle_sprintf_object_argumenttype                 : p->obj  = va_arg( va, void *); break;
      case mulle_sprintf_int64_t_argumenttype                : p->qt   = va_arg( va, int64_t); break;
      case mulle_sprintf_short_argumenttype                  : p->s    = (short) va_arg( va, int); break;  // promotion(!)
      case mulle_sprintf_signed_size_t_argumenttype          : p->sSt  = va_arg( va, ssize_t); break;
      case mulle_sprintf_size_t_argumenttype                 : p->St   = va_arg( va, size_t); break;
      case mulle_sprintf_size_t_pointer_argumenttype         : p->pSt  = va_arg( va, size_t *); break;
         //case mulle_sprintf_sse_argumenttype             : p->char = va_arg( va, char); break;
      case mulle_sprintf_uint64_t_argumenttype               : p->Qt   = va_arg( va, uint64_t); break;
      case mulle_sprintf_uint64_t_pointer_argumenttype       : p->pQt  = va_arg( va, uint64_t *); break;
      case mulle_sprintf_uintmax_t_argumenttype              : p->Imt  = va_arg( va, uintmax_t); break;
      case mulle_sprintf_uintmax_t_pointer_argumenttype      : p->pImt = va_arg( va, uintmax_t *); break;
      case mulle_sprintf_unsigned_char_argumenttype          : p->C    = (unsigned char) va_arg( va, int); break; // promotion(!)
      case mulle_sprintf_unsigned_char_pointer_argumenttype  : p->pC   = va_arg( va, unsigned char *); break;
      case mulle_sprintf_unsigned_int_argumenttype           : p->I    = va_arg( va, unsigned int); break;
      case mulle_sprintf_unsigned_int_pointer_argumenttype   : p->pI   = va_arg( va, unsigned int *); break;
      case mulle_sprintf_unsigned_long_argumenttype          : p->L    = va_arg( va, unsigned long); break;
      case mulle_sprintf_unsigned_long_long_argumenttype     : p->LL   = va_arg( va, unsigned long long); break;
      case mulle_sprintf_unsigned_long_pointer_argumenttype  : p->pL   = va_arg( va, unsigned long *); break;
      case mulle_sprintf_unsigned_ptrdiff_t_argumenttype     : p->Dif  = va_arg( va, unsigned MULLE_SPRINTF_PTRDIFF_TYPE); break;
      case mulle_sprintf_unsigned_short_argumenttype         : p->S    = (unsigned short) va_arg( va, unsigned int); break;  // promotion(!)
      case mulle_sprintf_unsigned_short_pointer_argumenttype : p->pS   = va_arg( va, unsigned short *); break;
      case mulle_sprintf_void_pointer_argumenttype           : p->pv   = va_arg( va, void *); break;
      case mulle_sprintf_wchar_pointer_argumenttype          : p->pwc  = va_arg( va, wchar_t *); break;
      case mulle_sprintf_wint_t_argumenttype                 : p->wc   = va_arg( va, wint_t); break;
      case mulle_sprintf_uint16_t_pointer_argumenttype       : p->pu16 = va_arg( va, uint16_t *); break;
      case mulle_sprintf_uint32_t_pointer_argumenttype       : p->pu32 = va_arg( va, uint32_t *); break;
      case mulle_sprintf_NSDecimal_pointer_argumenttype      : p->pDecimal = va_arg( va, struct _NSDecimal *); break;

      case mulle_sprintf_unsigned_long_long_pointer_argumenttype : p->pLL  = va_arg( va, unsigned long long *); break;
      case mulle_sprintf_unsigned_ptrdiff_t_pointer_argumenttype : p->pDif = va_arg( va, unsigned MULLE_SPRINTF_PTRDIFF_TYPE *); break;
      }
      ++type;
      ++p;
   }
}


void  mulle_mvsprintf_set_values( union mulle_sprintf_argumentvalue *p,
                                  mulle_sprintf_argumenttype_t  *type,
                                  unsigned int   n,
                                  mulle_vararg_list va)
{
   union mulle_sprintf_argumentvalue   *sentinel;

   sentinel = &p[ n];
   p        = &p[ 1];
   type     = &type[ 1];
   while( p < sentinel)
   {
      switch( *type)
      {
      default                                                : abort();

      case mulle_sprintf_int_argumenttype                    : p->i    = mulle_vararg_next_integer( va, int); break;

      case mulle_sprintf_char_argumenttype                   : p->c    = (char) mulle_vararg_next_integer( va, char); break;  // promotion(!)
      case mulle_sprintf_char_pointer_argumenttype           : p->pc   = mulle_vararg_next_pointer( va, char *); break;
      case mulle_sprintf_double_argumenttype                 : p->d    = mulle_vararg_next_fp( va, double); break;
      case mulle_sprintf_intmax_t_argumenttype               : p->imt  = mulle_vararg_next_integer( va, intmax_t); break;
      case mulle_sprintf_long_argumenttype                   : p->l    = mulle_vararg_next_integer( va, long); break;
      case mulle_sprintf_long_double_argumenttype            : p->ld   = mulle_vararg_next_fp( va, long double); break;
      case mulle_sprintf_long_long_argumenttype              : p->ll   = mulle_vararg_next_integer( va, long long); break;
      case mulle_sprintf_void_argumenttype                   : break;
      case mulle_sprintf_ptrdiff_t_argumenttype              : p->dif  = mulle_vararg_next_integer( va, ptrdiff_t); break;
      case mulle_sprintf_object_argumenttype                 : p->obj  = mulle_vararg_next_pointer( va, void *); break;
      case mulle_sprintf_int64_t_argumenttype                : p->qt   = mulle_vararg_next_integer( va, int64_t); break;
      case mulle_sprintf_short_argumenttype                  : p->s    = (short) mulle_vararg_next_integer( va, short); break;  // promotion(!)
      case mulle_sprintf_signed_size_t_argumenttype          : p->sSt  = mulle_vararg_next_integer( va, ssize_t); break;
      case mulle_sprintf_size_t_argumenttype                 : p->St   = mulle_vararg_next_integer( va, size_t); break;
      case mulle_sprintf_size_t_pointer_argumenttype         : p->pSt  = mulle_vararg_next_pointer( va, size_t *); break;
         //case mulle_sprintf_sse_argumenttype             : p->char = va_arg( va, char); break;
      case mulle_sprintf_uint64_t_argumenttype               : p->Qt   = mulle_vararg_next_integer( va, uint64_t); break;
      case mulle_sprintf_uint64_t_pointer_argumenttype       : p->pQt  = mulle_vararg_next_pointer( va, uint64_t *); break;
      case mulle_sprintf_uintmax_t_argumenttype              : p->Imt  = mulle_vararg_next_integer( va, uintmax_t); break;
      case mulle_sprintf_uintmax_t_pointer_argumenttype      : p->pImt = mulle_vararg_next_pointer( va, uintmax_t *); break;
      case mulle_sprintf_unsigned_char_argumenttype          : p->C    = (unsigned char) mulle_vararg_next_integer( va, unsigned char); break; // promotion(!)
      case mulle_sprintf_unsigned_char_pointer_argumenttype  : p->pC   = mulle_vararg_next_pointer( va, unsigned char *); break;
      case mulle_sprintf_unsigned_int_argumenttype           : p->I    = mulle_vararg_next_integer( va, unsigned int); break;
      case mulle_sprintf_unsigned_int_pointer_argumenttype   : p->pI   = mulle_vararg_next_pointer( va, unsigned int *); break;
      case mulle_sprintf_unsigned_long_argumenttype          : p->L    = mulle_vararg_next_integer( va, unsigned long); break;
      case mulle_sprintf_unsigned_long_long_argumenttype     : p->LL   = mulle_vararg_next_integer( va, unsigned long long); break;
      case mulle_sprintf_unsigned_long_pointer_argumenttype  : p->pL   = mulle_vararg_next_pointer( va, unsigned long *); break;
      case mulle_sprintf_unsigned_ptrdiff_t_argumenttype     : p->Dif  = mulle_vararg_next_integer( va, unsigned MULLE_SPRINTF_PTRDIFF_TYPE); break;
      case mulle_sprintf_unsigned_short_argumenttype         : p->S    = (unsigned short) mulle_vararg_next_integer( va, unsigned short); break;  // promotion(!)
      case mulle_sprintf_unsigned_short_pointer_argumenttype : p->pS   = mulle_vararg_next_pointer( va, unsigned short *); break;
      case mulle_sprintf_void_pointer_argumenttype           : p->pv   = mulle_vararg_next_pointer( va, void *); break;
      case mulle_sprintf_wchar_pointer_argumenttype          : p->pwc  = mulle_vararg_next_pointer( va, wchar_t *); break;
      case mulle_sprintf_wint_t_argumenttype                 : p->wc   = mulle_vararg_next_integer( va, wint_t); break;
      case mulle_sprintf_uint16_t_pointer_argumenttype       : p->pu16 = mulle_vararg_next_pointer( va, uint16_t *); break;
      case mulle_sprintf_uint32_t_pointer_argumenttype       : p->pu32 = mulle_vararg_next_pointer( va, uint32_t *); break;
      case mulle_sprintf_NSDecimal_pointer_argumenttype      : p->pDecimal = mulle_vararg_next_pointer( va, struct _NSDecimal *); break;

      case mulle_sprintf_unsigned_long_long_pointer_argumenttype : p->pLL  = mulle_vararg_next_pointer( va, unsigned long long *); break;
      case mulle_sprintf_unsigned_ptrdiff_t_pointer_argumenttype : p->pDif = mulle_vararg_next_pointer( va, unsigned MULLE_SPRINTF_PTRDIFF_TYPE *); break;
      }
      ++type;
      ++p;
   }
}


void  _mulle_sprintf_dump_available_conversion_characters( struct mulle_sprintf_conversion *table);
void  _mulle_sprintf_dump_available_conversion_characters( struct mulle_sprintf_conversion *table)
{
   // ' ' && c <= '~'
   char  c;
   int   i;

   for( c = ' '; c <= '~'; c++)
   {
      i = mulle_sprintf_index_for_character( c);
      if( i >= 0 && ! table->modifiers[ i] && ! table->jumps[ i])
         putchar( c);
   }
   putchar( '\n');
   fflush( stdout);
}


void  _mulle_sprintf_dump_available_defaultconversion_characters( void);
void  _mulle_sprintf_dump_available_defaultconversion_characters( void)
{
   _mulle_sprintf_dump_available_conversion_characters( mulle_sprintf_get_defaultconversion());
}


// static inline int   mulle_sprintf_is_flag_character( int c)
// {
//    return( strchr( "#0- +b'", c) != NULL);
// }
//

#pragma mark - functions

static int   _mulle_sprintf_register_functions( struct mulle_sprintf_conversion *table,
                                                struct mulle_sprintf_function *functions,
                                                mulle_sprintf_conversioncharacter_t c)
{
   struct mulle_sprintf_function   **p;
   int                             i;

   i = mulle_sprintf_index_for_character( c);
   if( i < 0)
      return( EINVAL);
   if( table->modifiers[ i])
      return( EEXIST);

   p  = &table->jumps[ i];

   // overwriting something, why not ?
   *p = functions;
   return( 0);
}


#pragma mark - functions API

int   mulle_sprintf_register_functions( struct mulle_sprintf_conversion *table,
                                        struct mulle_sprintf_function *functions,
                                        mulle_sprintf_conversioncharacter_t c)
{
   int   rval;

   if( ! functions)
   {
      errno = EINVAL;
      return( -1);
   }

   assert( functions->determine_argument_type);
   assert( functions->convert_argument);

   if( ! table)
      table = mulle_sprintf_get_defaultconversion();

   rval = _mulle_sprintf_register_functions( table, functions, c);
   if( rval)
   {
      errno = rval;
      return( -1);
   }
   return( 0);
}


int   mulle_sprintf_register_default_functions( struct mulle_sprintf_function *functions,
                                                mulle_sprintf_conversioncharacter_t c)
{
   return( mulle_sprintf_register_functions( mulle_sprintf_get_defaultconversion(),
                                             functions,
                                             c));
}


#pragma mark - modifiers


static int   _mulle_sprintf_register_modifier( struct mulle_sprintf_conversion *table,
                                               mulle_sprintf_modifiercharacter c)
{
   int   i;

   i = mulle_sprintf_index_for_character( c);
   if( i < 0)
      return( EINVAL);
   if( table->jumps[ i])
      return( EEXIST);

   //
   // just mark as used, override is OK, since modifiers are
   // shared
   //
   table->modifiers[ i] = c;
   return( 0);
}


static int   _mulle_sprintf_register_modifiers( struct mulle_sprintf_conversion *table,
                                                mulle_sprintf_modifiercharacter *s)
{
   int   rval;
   int   c;

   rval = 0;
   while( (c = *s++))
   {
      rval = _mulle_sprintf_register_modifier( table, (char) c);
      if( rval)
         break;
   }
   return( rval);
}



#pragma mark - modifiers API


int   mulle_sprintf_register_modifiers( struct mulle_sprintf_conversion *table,
                                        mulle_sprintf_modifiercharacter *s)
{
   int   rval;

   if( ! s)
   {
      errno = EINVAL;
      return( -1);
   }

   if( ! table)
      table = mulle_sprintf_get_defaultconversion();

   rval = _mulle_sprintf_register_modifiers( table, s);
   if( rval)
   {
      errno = rval;
      return( -1);
   }
   return( 0);
}


int   mulle_sprintf_register_modifier( struct mulle_sprintf_conversion *table,
                                       mulle_sprintf_modifiercharacter c)
{
   int   rval;

   if( ! table)
      table = mulle_sprintf_get_defaultconversion();

   rval = _mulle_sprintf_register_modifier( table, c);
   if( rval)
   {
      errno = rval;
      return( -1);
   }
   return( 0);
}


int   mulle_sprintf_register_default_modifier( mulle_sprintf_modifiercharacter c)
{
   return( mulle_sprintf_register_modifier( NULL, c));
}


int   mulle_sprintf_register_default_modifiers( mulle_sprintf_modifiercharacter *s)
{
   return( mulle_sprintf_register_modifiers( NULL, s));
}

//
// this not only contains length modifiers but also flag characters
//
int   mulle_sprintf_register_standardmodifiers( struct mulle_sprintf_conversion *table)
{
   return( mulle_sprintf_register_modifiers( table, "0123456789.#- +\'b*$"));
}



MULLE_C_CONSTRUCTOR( mulle_sprintf_register_default_modifiers_on_load)
void  mulle_sprintf_register_default_modifiers_on_load()
{
   mulle_sprintf_register_standardmodifiers( NULL);
}

