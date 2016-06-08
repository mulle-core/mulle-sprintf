/*
 *  struct mulle_sprintf_function.c
 *  MulleFoundation
 *
 *  Created by Nat! on 23.10.11.
 *  Copyright 2011 Mulle kybernetiK. All rights reserved.
 *
 */
#ifndef __need_wint_t
#define __need_wint_t
#endif

#include "mulle_sprintf_function.h"

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <mulle_vararg/mulle_vararg.h>


#define SIZEOF( type)  sizeof( type) < sizeof( int) ? sizeof( int) : sizeof( type)

struct mulle_sprintf_conversion   mulle_sprintf_defaultconversion;


// order must match enum!

unsigned char   mulle_sprintf_argumentsize[] = 
{
   SIZEOF( int),

   SIZEOF( char),                   // promotion(!)
   SIZEOF( char *),
   SIZEOF( double),
   SIZEOF( intmax_t),

   SIZEOF( long),
   SIZEOF( long double),
   SIZEOF( long long),
   SIZEOF( void *),
   
   SIZEOF( ptrdiff_t),
   SIZEOF( int64_t),
   SIZEOF( short),
   SIZEOF( size_t),

   SIZEOF( size_t *),
   SIZEOF( uint64_t),
   SIZEOF( uint64_t *),
   SIZEOF( uintmax_t),

   SIZEOF( uintmax_t *),
   SIZEOF( unsigned char),
   SIZEOF( unsigned char *),
   SIZEOF( unsigned int),

   SIZEOF( unsigned int *),
   SIZEOF( unsigned long),
   SIZEOF( unsigned long long),
   SIZEOF( unsigned long long *),

   SIZEOF( unsigned long *),
   SIZEOF( unsigned __PTRDIFF_TYPE__),
   SIZEOF( unsigned __PTRDIFF_TYPE__ *),
   SIZEOF( unsigned short),

   SIZEOF( unsigned short *),
   0,    // vector
   0,    // void
   SIZEOF( void *),

   SIZEOF( wchar_t *),
   SIZEOF( wint_t *),
   SIZEOF( struct _NSDecimal *),
   0,
};


// needs to be done in  a loop here, because va_arg calls can't be separated
// across function calls (interestingly)
//
void  mulle_vsprintf_set_values( union mulle_sprintf_argumentvalue *p,     
                                 mulle_sprintf_argumenttype_t  *type, 
                                 unsigned int   n,
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
      default                                             : abort();
         
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
      case mulle_sprintf_unsigned_ptrdiff_t_argumenttype     : p->Dif  = va_arg( va, unsigned __PTRDIFF_TYPE__); break;
      case mulle_sprintf_unsigned_short_argumenttype         : p->S    = (unsigned short) va_arg( va, unsigned int); break;  // promotion(!)
      case mulle_sprintf_unsigned_short_pointer_argumenttype : p->pS   = va_arg( va, unsigned short *); break;
      case mulle_sprintf_void_pointer_argumenttype           : p->pv   = va_arg( va, void *); break;
      case mulle_sprintf_wchar_pointer_argumenttype          : p->pwc  = va_arg( va, wchar_t *); break;
      case mulle_sprintf_NSDecimal_pointer_argumenttype      : p->pDecimal = va_arg( va, struct _NSDecimal *); break;
         //case mulle_sprintf_wint_t_argumenttype                 : p->wit  = va_arg( va, wint_t); break;
         
      case mulle_sprintf_unsigned_long_long_pointer_argumenttype : p->pLL  = va_arg( va, unsigned long long *); break;
      case mulle_sprintf_unsigned_ptrdiff_t_pointer_argumenttype : p->pDif = va_arg( va, unsigned __PTRDIFF_TYPE__ *); break;
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
         
      case mulle_sprintf_char_argumenttype                   : p->c    = (char) mulle_vararg_next_integer( va, int); break;  // promotion(!)
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
      case mulle_sprintf_short_argumenttype                  : p->s    = (short) mulle_vararg_next_integer( va, int); break;  // promotion(!)
      case mulle_sprintf_size_t_argumenttype                 : p->St   = mulle_vararg_next_integer( va, size_t); break;
      case mulle_sprintf_size_t_pointer_argumenttype         : p->pSt  = mulle_vararg_next_pointer( va, size_t *); break;
         //case mulle_sprintf_sse_argumenttype             : p->char = va_arg( va, char); break;
      case mulle_sprintf_uint64_t_argumenttype               : p->Qt   = mulle_vararg_next_integer( va, uint64_t); break;
      case mulle_sprintf_uint64_t_pointer_argumenttype       : p->pQt  = mulle_vararg_next_pointer( va, uint64_t *); break;
      case mulle_sprintf_uintmax_t_argumenttype              : p->Imt  = mulle_vararg_next_integer( va, uintmax_t); break;
      case mulle_sprintf_uintmax_t_pointer_argumenttype      : p->pImt = mulle_vararg_next_pointer( va, uintmax_t *); break;
      case mulle_sprintf_unsigned_char_argumenttype          : p->C    = (unsigned char) mulle_vararg_next_integer( va, int); break; // promotion(!)
      case mulle_sprintf_unsigned_char_pointer_argumenttype  : p->pC   = mulle_vararg_next_pointer( va, unsigned char *); break;
      case mulle_sprintf_unsigned_int_argumenttype           : p->I    = mulle_vararg_next_integer( va, unsigned int); break;
      case mulle_sprintf_unsigned_int_pointer_argumenttype   : p->pI   = mulle_vararg_next_pointer( va, unsigned int *); break;
      case mulle_sprintf_unsigned_long_argumenttype          : p->L    = mulle_vararg_next_integer( va, unsigned long); break;
      case mulle_sprintf_unsigned_long_long_argumenttype     : p->LL   = mulle_vararg_next_integer( va, unsigned long long); break;
      case mulle_sprintf_unsigned_long_pointer_argumenttype  : p->pL   = mulle_vararg_next_pointer( va, unsigned long *); break;
      case mulle_sprintf_unsigned_ptrdiff_t_argumenttype     : p->Dif  = mulle_vararg_next_integer( va, unsigned __PTRDIFF_TYPE__); break;
      case mulle_sprintf_unsigned_short_argumenttype         : p->S    = (unsigned short) mulle_vararg_next_integer( va, unsigned int); break;  // promotion(!)
      case mulle_sprintf_unsigned_short_pointer_argumenttype : p->pS   = mulle_vararg_next_pointer( va, unsigned short *); break;
      case mulle_sprintf_void_pointer_argumenttype           : p->pv   = mulle_vararg_next_pointer( va, void *); break;
      case mulle_sprintf_wchar_pointer_argumenttype          : p->pwc  = mulle_vararg_next_pointer( va, wchar_t *); break;
      case mulle_sprintf_NSDecimal_pointer_argumenttype      : p->pDecimal = mulle_vararg_next_pointer( va, struct _NSDecimal *); break;
         //case mulle_sprintf_wint_t_argumenttype                 : p->wit  = mulle_vararg_next_integer( va, wint_t); break;
         
      case mulle_sprintf_unsigned_long_long_pointer_argumenttype : p->pLL  = mulle_vararg_next_pointer( va, unsigned long long *); break;
      case mulle_sprintf_unsigned_ptrdiff_t_pointer_argumenttype : p->pDif = mulle_vararg_next_pointer( va, unsigned __PTRDIFF_TYPE__ *); break;
      }
      ++type;
      ++p;
   }
}


#if DEBUG
#define return_abort( x)   abort()
# else
#define return_abort( x)   return( x)
#endif


int   _mulle_sprintf_register_modifier( struct mulle_sprintf_conversion *table, 
                                        mulle_sprintf_modifiercharacter c)
{
   unsigned int   i;

   if( c < ' ' || c > '~')
      return_abort( -3);

   i = mulle_sprintf_index_for_character( c);
   if( table->jumps[ i])
      return_abort( -1);
   
   //
   // just mark as used, override is OK, since modifiers are
   // shared
   //
   table->modifiers[ i] = c;
   return( 0);
}  


int  _mulle_sprintf_register_functions( struct mulle_sprintf_conversion *table, 
                                        struct mulle_sprintf_function *functions,
                                        mulle_sprintf_conversioncharacter_t c)
{
   struct mulle_sprintf_function   **p;
   unsigned int          i;

   if( c < '!' || c > '~')
      return_abort( -3);

   i = mulle_sprintf_index_for_character( c);
   if( table->modifiers[ i])
      return_abort( -2);
      
   p = &table->jumps[ i];
   if( *p)
      return_abort( -1);

   *p = functions;
   return( 0);
}  


                                                                       
int  _mulle_sprintf_register_modifiers( struct mulle_sprintf_conversion *table, mulle_sprintf_modifiercharacter *s)
{
   int   rval;
   int   c;
      
   rval = 0;
   while( c = *s++)
   {
      rval = _mulle_sprintf_register_modifier( table, (char) c);
      if( rval)
         break;
   }
   return( rval);
}


int   _mulle_sprintf_register_standardmodifiers( struct mulle_sprintf_conversion *table)
{
   return( _mulle_sprintf_register_modifiers( table, "0123456789.#- +\'*$"));
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
      if( ! table->modifiers[ i] && ! table->jumps[ i])
         putchar( c);
   }
   putchar( '\n');
   fflush( stdout);
}


void  _mulle_sprintf_dump_available_defaultconversion_characters( void);
void  _mulle_sprintf_dump_available_defaultconversion_characters( void)
{
   _mulle_sprintf_dump_available_conversion_characters( &mulle_sprintf_defaultconversion);
}


__attribute__((constructor))
void  mulle_sprintf_register_default_modifiers_on_load()
{
   _mulle_sprintf_register_standardmodifiers( &mulle_sprintf_defaultconversion);
}
