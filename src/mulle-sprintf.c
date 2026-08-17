//
//  mulle-sprintf.c
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
#pragma clang diagnostic ignored "-Wparentheses"

#include "mulle-sprintf.h"
#include "mulle-sprintf-function.h"

#include "include-private.h"

#include <errno.h>
#include <stdint.h>
#include <string.h>

// Set to 0 for a baseline that sends the complete format directly through
// setup_context/parse_context. Set to 1 for the static-prefix scanner and
// bounded SWAR fast path.
#ifndef MULLE_SPRINTF_PREFIX_OPTIMIZED
# define MULLE_SPRINTF_PREFIX_OPTIMIZED 1
#endif

#ifndef HAVE_SPRINTF_BOOL
# define HAVE_SPRINTF_BOOL  1
#endif

#ifndef MULLE_SPRINTF_PREFIX_SWAR
# define MULLE_SPRINTF_PREFIX_SWAR 1
#endif

// Set to 0 to scan only through the static prefix. Set to 1 to retain
// the full-length scan required by the bounded SWAR follow-up scan.
#ifndef MULLE_SPRINTF_PREFIX_SCAN_FULL_LENGTH
# define MULLE_SPRINTF_PREFIX_SCAN_FULL_LENGTH 0
#endif

#define STACKABLE_N             16
#define STACKABLE_ARGUMENTS     (STACKABLE_N + 1)   // leave a dummy argument #0

//
// now here C++ & templates would come in handy...
// this is code is all original, except for some of the
// <atype>_<format>_conversion codes, which are lifted
// from sqlite with is Public Domain
//
typedef enum
{
   state_begin,
   state_opt_flags,
   state_opt_separators,
   state_argument_index,
   state_width,
   state_precision,
   state_modifier,
   state_conversion
} parser_state;


#pragma mark - thread local storage


//
// per default, this is stored in a global variable
// when "threading" gets linked it, this will be
// stored thread locally
// TODO: maybe put in a '\n' replacement, to expand '\n' into '\r\n'
//       this could also help with indentation to expand '\n' into '\n\t\t'
//       maybe ?
struct mulle_sprintf_malloc_storage
{
   const char                                  **starts;
   size_t                                      s_starts;
   struct mulle_sprintf_argumentarray          arguments;
   struct mulle_sprintf_formatconversioninfo   *infos;
   size_t                                      s_infos;
   struct mulle_allocator                      *allocator;
};


static struct mulle_sprintf_malloc_storage
   *mulle_sprintf_malloc_storage_create( struct mulle_allocator *allocator)
{
   struct mulle_sprintf_malloc_storage   *storage;

   storage = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle_sprintf_malloc_storage));
   storage->allocator = allocator;
   return( storage);
}


static void
   mulle_sprintf_malloc_storage_done( struct mulle_sprintf_malloc_storage *storage)
{
   mulle_allocator_free( storage->allocator, (void *) storage->starts);
   mulle_allocator_free( storage->allocator, storage->infos);
   mulle_allocator_free( storage->allocator, storage->arguments.types);
   mulle_allocator_free( storage->allocator, storage->arguments.values);
}


static void
   mulle_sprintf_malloc_storage_free( struct mulle_sprintf_malloc_storage *storage)
{
   struct mulle_sprintf_config   *config;

   config = mulle_sprintf_get_config();

   mulle_sprintf_malloc_storage_done( storage);
   mulle_allocator_free( storage->allocator, storage);
   mulle_thread_tss_free( config->key);
   config->key = (mulle_thread_tss_t) -1;
}


static void   *get_storage( struct mulle_allocator *allocator)
{
   struct mulle_sprintf_malloc_storage   *storage;
   struct mulle_sprintf_config           *config;

   config = mulle_sprintf_get_config();
   if( config->key == (mulle_thread_tss_t) -1)
   {
      if( mulle_thread_tss_create( (void *) config->free_storage, &config->key))
      {
         perror( "mulle_thread_tss_create");
         abort();
      }
   }

   storage = mulle_thread_tss_get( config->key);
   if( ! storage)
   {
      storage = mulle_sprintf_malloc_storage_create( allocator);
      mulle_thread_tss_set( config->key, storage);
   }
   return( storage);
}


static void   free_storage( void)
{
   struct mulle_sprintf_malloc_storage   *storage;
   struct mulle_sprintf_config           *config;

   config = mulle_sprintf_get_config();
   if( config->key == (mulle_thread_tss_t) -1)
      return;

   storage = mulle_thread_tss_get( config->key);
   if( ! storage)
      return;

   mulle_sprintf_malloc_storage_free( storage);
   mulle_thread_tss_set( config->key, NULL);
}


MULLE__SPRINTF_GLOBAL_VAR
struct mulle_sprintf_config    mulle_sprintf_config =
{
   (mulle_thread_tss_t) -1,
   get_storage,
   free_storage,
   { { 0}, { 0 } }
};


void   mulle_sprintf_register_default_conversion_functions_if_needed( struct mulle_sprintf_conversion *conversion)
{
   if( conversion->jumps[ mulle_sprintf_index_for_character( 'd')])
      return;

   mulle_sprintf_register_character_functions( conversion);
   mulle_sprintf_register_escape_functions( conversion);
   mulle_sprintf_register_fp_functions( conversion);
   mulle_sprintf_register_integer_functions( conversion);
   mulle_sprintf_register_pointer_functions( conversion);
   mulle_sprintf_register_return_functions( conversion);
   mulle_sprintf_register_string_functions( conversion);
   mulle_sprintf_register_standardmodifiers( conversion);
}


static void   *space_for_starts( unsigned int n, struct mulle_allocator *allocator)
{
   struct mulle_sprintf_malloc_storage   *storage;
   struct mulle_sprintf_config           *config;
   size_t                                size;

   config  = mulle_sprintf_get_config();
   storage = (*config->get_storage)( allocator);

   size = n * sizeof( char *);
   if( size > storage->s_starts)
   {
      storage->starts   = mulle_allocator_realloc( allocator, (void *) storage->starts, size);
      storage->s_starts = size;
   }
   return( (void *) storage->starts);
}


static struct mulle_sprintf_argumentarray   *
   space_for_arguments( unsigned int n,
                        struct mulle_allocator *allocator)
{
   struct mulle_sprintf_malloc_storage   *storage;
   struct mulle_sprintf_config           *config;
   struct mulle_sprintf_argumentarray    *args;

   config  = mulle_sprintf_get_config();
   storage = (*config->get_storage)( allocator);

   ++n;  // leave one empty at 0

   args = &storage->arguments;
   if( n > args->size)
   {
      args->values = mulle_allocator_realloc( allocator,
                                              args->values,
                                              n * sizeof( union mulle_sprintf_argumentvalue));
      args->types  = mulle_allocator_realloc( allocator,
                                              args->types,
                                              n * sizeof( unsigned char));
      args->size   = n;
   }
   return( args);
}


static void   *space_for_infos( unsigned int n, struct mulle_allocator *allocator)
{
   struct mulle_sprintf_malloc_storage   *storage;
   struct mulle_sprintf_config           *config;
   size_t                                size;

   config  = mulle_sprintf_get_config();
   storage = (*config->get_storage)( allocator);

   ++n;
   size = n * sizeof( struct mulle_sprintf_formatconversioninfo);
   if( size > storage->s_infos)
   {
      storage->infos   = mulle_allocator_realloc( allocator, storage->infos, size);
      storage->s_infos = size;
   }
   return( storage->infos);
}


#pragma mark - conversion tables


static inline struct mulle_sprintf_function   *
   functions_for_conversion( mulle_sprintf_vector_t jumptable,
                             mulle_sprintf_conversioncharacter_t c)
{
   int    i;

   i = mulle_sprintf_index_for_character( c);
   return( i < 0 ? NULL : jumptable[ i]);
}


/* c is the character past '%' */
static int
   determine_is_valid_conversion_character( struct mulle_sprintf_conversion *table,
                                            mulle_sprintf_conversioncharacter_t c)
{
   struct mulle_sprintf_function  *functions;

   if( mulle_sprintf_is_modifier_character( table->modifiers, c))
      return( 0);

   switch( c)
   {
   case '%' : return( 1);
   default  : functions = functions_for_conversion( table->jumps, c);
              return( functions ? 1 : -1);
   }
}


static inline mulle_sprintf_argumenttype_t
   jump_determine_argument_type( struct mulle_sprintf_formatconversioninfo *info)
{
   struct mulle_sprintf_function  *functions;

   // callers must have resolved the conversion already (see parse_all_conversions)
   assert( info->function);

   functions = info->function;
   return( (*functions->determine_argument_type)( info));
}


static inline int
   jump_convert_argument( struct mulle_buffer *buffer,
                          struct mulle_sprintf_formatconversioninfo *info,
                          struct mulle_sprintf_argumentarray *arguments,
                          int i)
{
   struct mulle_sprintf_function  *functions;

   // callers must have resolved the conversion already (see parse_all_conversions)
   assert( info->function);

   functions = info->function;
   return( (*functions->convert_argument)( buffer, info, arguments, i));
}


static inline mulle_sprintf_argumenttype_t
   determine_argument_type( struct mulle_sprintf_formatconversioninfo *info)
{
   // need to code this (possibly :))
   if( info->modifier[ 0] == 'v' ||
       info->modifier[ 1] == 'v' ||
       info->modifier[ 2] == 'v')
      return( mulle_sprintf_vector_argumenttype);

   return( jump_determine_argument_type( info));
}


static inline int
   convert_argument( struct mulle_buffer *buffer,
                     struct mulle_sprintf_formatconversioninfo *info,
                     struct mulle_sprintf_argumentarray *arguments,
                     int before)
{
   if( info->modifier[ 0] == 'v' ||
       info->modifier[ 1] == 'v' ||
       info->modifier[ 2] == 'v')
   {
      mulle_buffer_add_string( buffer, "<vector unsupported>");
      return( -1);
   }

   // width / precision from '*' use their (pre-resolved) argument indices
   if( info->width_argument)
   {
      info->width = arguments->values[ info->width_argument].i;
      if( info->width < 0)
      {
         info->memory.minus_found = 1;
         info->width              = - info->width;
      }
   }

   if( info->memory.minus_found)
   {
      info->memory.left_justify = 1;
      info->memory.zero_found   = 0;
   }

   if( info->precision_argument)
   {
      info->precision = arguments->values[ info->precision_argument].i;
      if( info->precision < 0)
         info->precision = - info->precision;
   }

   info->mystery = (void *) (intptr_t) before;  // for return conversion
   return( jump_convert_argument( buffer,
                                  info,
                                  arguments,
                                  info->value_argument));
}


typedef struct
{
   const char    *start;
   const char    *curr;
   const char    *memo;
   const char    *sentinel;

   parser_state   state;

   int     modifier_index;
} format_conversion_parser;


//
// expect parser->curr to point at '$'
// expect parser->memo to point after '*' (first digit)
//
static inline int
   positive_int_value_from_memo( format_conversion_parser *parser,
                                 struct mulle_sprintf_formatconversioninfo *info)

{
   const char   *p;
   int    digit;
   int    value;


   MULLE_C_UNUSED( info);

   p = parser->memo;
   if( p == parser->curr)
      return( 0);  // it's OK!

   value = 0;
   while( p < parser->curr)
   {
      if( *p < '0' || *p > '9')
         return( -1);

      digit = *p - '0';
      if( value > (2147483647 - digit) / 10)
         return( -1);  // overflow

      value  = value * 10 + digit;
      ++p;
   }

   return( value);
}


//
// returns: next argument index
//          and max used argument (for indexes maybe in max_arg)
//
static inline int
   parse_conversion_info( const char *format,
                          struct mulle_sprintf_formatconversioninfo *info,
                          int arg,
                          int *max_arg,
                          mulle_sprintf_modifier_t modifier_table)
{
   format_conversion_parser  parser;
   char                      c;
   int                       value;

   memset( info, 0, sizeof( struct mulle_sprintf_formatconversioninfo));

   parser.memo           = NULL;
   parser.curr           = format;
   parser.sentinel       = &format[ 127];  // %2147483647$0#- +\[,;:_]2147483647.2147483647hllX"

   parser.memo           = NULL;
   parser.state          = state_begin;

   parser.modifier_index = 0;

   while( (c = *++parser.curr))
   {
      // bail on obvious syntax error
      if( parser.curr == parser.sentinel)
         return( -1);

      switch( parser.state)
      {
      case state_begin :
         parser.memo = NULL;
         if( c >= '1' && c <= '9')
         {
            parser.memo = parser.curr;
            parser.state = state_argument_index;  // or maybe width later
            continue;
         }
         parser.state = state_opt_flags;  // fall thru

      case state_opt_flags :
         switch( c)
         {
         case ' '  : info->memory.space_found = 1; continue;
         case '0'  : info->memory.zero_found  = 1; continue;
         case '#'  : info->memory.hash_found  = 1; continue;
         case '-'  : info->memory.minus_found = 1; continue;
         case '+'  : info->memory.plus_found  = 1; continue;
         case '\'' : info->memory.quote_found = 1; continue;
#if HAVE_SPRINTF_BOOL
         case 'b'  : info->memory.bool_found  = 1; continue;
#endif
         }
         parser.state = state_opt_separators;  // fall thru

      case state_opt_separators :
         parser.state = state_width;  // next time
         if( c == ',' || c == ';' || c == ':' || c == '_')
         {
            info->separator = c;
            continue;
         }
         goto state_width_entry;

      case state_argument_index :
         if( c >= '0' && c <= '9')
            continue;
         if( c == '$')
         {
            value = positive_int_value_from_memo( &parser, info);
            if( value <= 0)
               return( -1);

            info->argv_index[ 0]              = value;
            info->memory.argument_index_found = 1;
            parser.memo                       = NULL;
            parser.state                      = state_opt_flags;
            arg                               = value;
            if( arg > *max_arg)
               *max_arg = arg;
            continue;
         }
         // asume it's width

         parser.state = state_width; // fall thru

      case state_width :
state_width_entry:
         if( ! parser.memo)  // first time ?
         {
            if( c == '*')
            {
               info->memory.width_found       = 1;
               info->memory.width_is_argument = 1;

               parser.memo = parser.curr + 1;
               continue;
            }

            if( c >= '1' && c <= '9')
            {
               info->memory.width_found = 1;

               parser.memo = parser.curr;
               continue;
            }
            // hmm not a width
         }
         else
         {
            if( c >= '0' && c <= '9')
               continue;

            if( c == '$')
            {
               if( ! info->memory.width_is_argument)
                  return( -1);

               value = positive_int_value_from_memo( &parser, info);
               if( value <= 0)
                  return( -1);

               info->argv_index[ 1]                   = value;
               info->width_argument                   = value;
               info->memory.width_is_indexed_argument = 1;
               parser.memo                            = NULL;
               parser.state                           = state_precision;
               arg                                    = value;
               if( arg > *max_arg)
                  *max_arg = arg;
               ++arg;  // dial up for conversion
               continue;
            }

            if( ! info->memory.width_is_argument)
            {
               info->width = positive_int_value_from_memo( &parser, info);
               if( info->width < 0)
                  return( -1);
               info->memory.width_found = 1;
            }
            else
            {
               if( parser.memo[ -1] != '*')
                  return( -1);
               if( arg > *max_arg)
                  *max_arg = arg;
               info->width_argument = arg;
               ++arg;  // dial up for conversion
            }
         }
         parser.state = state_precision;
         parser.memo  = NULL;
         MULLE_C_FALLTHROUGH;
         // after a precision, dot all digits belong to precision
      case state_precision :
         if( ! parser.memo)
         {
            if( c == '.')
            {
               info->memory.precision_found = 1;
               parser.memo = parser.curr + 1;
               continue;
            }
         }
         else
         {
            if( c == '*')
            {
               if( info->memory.precision_is_argument)
                  return( -1);

               info->memory.precision_is_argument = 1;
               parser.memo  = parser.curr + 1;
               continue;
            }
            if( c >= '0' && c <= '9')
               continue;

            if( c == '$')
            {
               if( ! info->memory.precision_is_argument)
                  return( -1);

               value = positive_int_value_from_memo( &parser, info);
               if( value <= 0)
                  return( -1);

               info->memory.precision_is_indexed_argument = 1;
               info->argv_index[ 2]                       = value;
               info->precision_argument                   = value;
               parser.state                               = state_modifier;
               arg                                        = value;
               if( arg > *max_arg)
                  *max_arg = arg;
               ++arg;  // dial up for conversion
               continue;
               // parser.memo = NULL;
            }

            if( ! info->memory.precision_is_argument)
            {
               value = positive_int_value_from_memo( &parser, info);
               if( value < 0)
                  return( -1);

               info->precision = value;
            }
            else
            {
               if( parser.memo[ -1] != '*')
                  return( -1);
               if( arg > *max_arg)
                  *max_arg = arg;
               info->precision_argument = arg;
               ++arg;  // dial up for conversion
            }
               // parser.memo = NULL;
         }
         parser.state = state_modifier;  // fall thru

      case state_modifier :
         // TODO: no _real_ need to hardcode this or ?
         if( mulle_sprintf_is_modifier_character( modifier_table, c))
         {
            if( parser.modifier_index >= 3)
               return( -1);

            info->modifier[ parser.modifier_index++] = c;
            continue;
         }
         parser.state = state_conversion; // fall thru

      case state_conversion :
         info->conversion = c;
         info->length     = (int) ((parser.curr - format) + 1);
         info->memory.pure = ( info->memory.argument_index_found == 0 &&
                               info->memory.zero_found == 0 &&
                               info->memory.minus_found == 0 &&
                               info->memory.space_found == 0 &&
                               info->memory.hash_found == 0 &&
                               info->memory.plus_found == 0 &&
                               info->memory.quote_found == 0 &&
                               info->memory.bool_found == 0 &&
                               info->memory.width_found == 0 &&
                               info->memory.precision_found == 0 &&
                               info->separator == 0 &&
                               info->modifier[ 0] == 0 &&
                               info->modifier[ 1] == 0 &&
                               info->modifier[ 2] == 0);
         if( info->memory.argument_index_found)
            info->value_argument = info->argv_index[ 0];
         else
            info->value_argument = arg;
         if( arg > *max_arg)
            *max_arg = arg;
         return( arg + 1);
      }
   }
   return( -1);
}





#pragma mark - mulle_vararg_list

//
// stuff we try to store on the stack, if possible
// if stack is small, reduce accordingly
// then more malloc memory is used


struct mulle_sprintf_context
{
   const char                                  **starts;
   const char                                  *startsBuf[ STACKABLE_N];
   const char                                  *format_end;
   struct mulle_sprintf_argumentarray          *arguments;
   struct mulle_sprintf_argumentarray          argumentBuf;
   struct mulle_sprintf_formatconversioninfo   *infos;
   struct mulle_sprintf_formatconversioninfo   conversionBuf[ STACKABLE_N];  // this is the biggy
   union mulle_sprintf_argumentvalue           valueBuf[ STACKABLE_ARGUMENTS];
   unsigned char                               typesBuf[ STACKABLE_ARGUMENTS];
   int                                         n;
};


static int
   determine_all_conversion_argument_types( struct mulle_sprintf_context *ctxt,
                                            struct mulle_sprintf_conversion *table)
{
   struct mulle_sprintf_formatconversioninfo   *info;
   int                                         i;

   // argument indices and types were resolved during parsing, so this is
   // now a pure scatter. All slots were preset to Int already, except the
   // width/precision '*' arguments which are integers anyway.
   for( i = 0; i < ctxt->n; i++)
   {
      info = &ctxt->infos[ i];
      ctxt->arguments->types[ info->value_argument] = info->value_type;
      if( info->value_type == (unsigned char) (mulle_sprintf_argumenttype_t) - 1)
         return( -4);
   }

   return( 0);
}



#if MULLE_SPRINTF_PREFIX_OPTIMIZED

#if MULLE_SPRINTF_PREFIX_SCAN_FULL_LENGTH

//
// SWAR (SIMD-within-a-register) helper to find a byte in a word. Used by
// scan_static_prefix_bounded, where the known remaining length guarantees
// that full words are only read while they are entirely within the string
// (no read past the NUL). A candidate hit is verified byte-wise, so the
// word check may only be conservative, never wrong.
//
static inline uint64_t   has_zero_byte_swar( uint64_t v)
{
   return( (v - 0x0101010101010101ULL) & ~v & 0x8080808080808080ULL);
}


static inline int   contains_byte_swar( uint64_t word, unsigned char c)
{
   return( has_zero_byte_swar( word ^ (0x0101010101010101ULL * c)) != 0);
}


//
// scans a format from its start for the static prefix: all bytes before
// the first '%' (or the whole string, if there is none). This is a pure
// function, it does not touch the buffer. Returns the prefix length and
// stores the full string length (up to the NUL) in *total.
//
// The first scan is byte-wise on purpose: no length is known yet, and a
// word-at-a-time scan of an unknown-length string would read past the NUL
// (not valgrind-clean). Once *total is known, follow-up scans of the
// remainder can use scan_static_prefix_bounded below.
//
static size_t   scan_static_prefix( const char *format, size_t *total)
{
   const char   *p;
   const char   *prefix;

   p      = format;
   prefix = NULL;

   while( *p)
   {
      if( ! prefix && *p == '%')
         prefix = p;
      ++p;
   }
   *total = (size_t)( p - format);

   return( prefix ? (size_t)( prefix - format) : (size_t)( p - format));
}


//
// scans for the static prefix within [p, p + remaining): all bytes before
// the first '%', or everything if there is none. The known length makes
// 8-byte word-at-a-time reads safe: no read ever passes p + remaining, so
// nothing past the NUL is touched (valgrind-clean).
//
static size_t   scan_static_prefix_bounded( const char *p, size_t remaining)
{
   size_t     i;
#if MULLE_SPRINTF_PREFIX_SWAR
   uint64_t   word;
#endif

   i = 0;
#if MULLE_SPRINTF_PREFIX_SWAR
   while( remaining - i >= 8)
   {
      memcpy( &word, p + i, sizeof( word));
      if( contains_byte_swar( word, '%'))
      {
         while( p[ i] != '%')
            ++i;
         break;
      }
      i += 8;
   }
#endif
   while( i < remaining && p[ i] != '%')
      ++i;

   return( i);
}

#else

//
// scans only until the next '%' or the terminating NUL. Unlike the
// full-length variant, this does not inspect the format after the prefix.
//
static size_t   scan_static_prefix( const char *format)
{
   const char   *p;

   p = format;
   while( *p && *p != '%')
      ++p;
   return( (size_t)( p - format));
}

#endif


//
// copies a static prefix of the format (literal text and, optionally,
// "%%" escapes) directly into the buffer. Only the remainder (starting at
// the first real conversion) needs the full conversion treatment. Formats
// with a large literal prefix avoid the parse/argument machinery entirely
// for that part; when the whole format is static, no conversion machinery
// runs at all.
//
// the "%%" shortcut can be gated on '%' still being the standard escape
// function (a custom '%' function must see the conversion): flip the #if
// to compare both variants
//
// returns: pointer to the first character of the remainder (a '%' or NUL)
//
static const char  *copy_static_prefix( struct mulle_buffer *buffer,
                                  const char *format,
                                  struct mulle_sprintf_conversion *table)
{
   const char     *p;
   size_t   prefix;
   int      escape_ok;
#if MULLE_SPRINTF_PREFIX_SCAN_FULL_LENGTH
   size_t   remaining;
   size_t   total;
#endif

   // Conversion-first formats have no static prefix. Avoid the scan and
   // enter the normal parser directly; retain the special handling for %%.
   if( format[ 0] == '%' && format[ 1] != '%')
      return( format);

   escape_ok = (functions_for_conversion( table->jumps, '%') ==
                &mulle_sprintf_escape_functions);

#if MULLE_SPRINTF_PREFIX_SCAN_FULL_LENGTH
   prefix    = scan_static_prefix( format, &total);
   remaining = total;
   p         = format;

   for(;;)
   {
      if( prefix)
         mulle_buffer_add_bytes( buffer, p, prefix);

      p         += prefix;
      remaining -= prefix;
      if( ! remaining)
         return( p);   // whole format was static

      if( ! escape_ok || remaining < 2 || p[ 1] != '%')
         return( p);   // a real conversion (or a stray '%') starts here

      // static escape "%%": emit a single '%' and continue with the
      // remainder, whose length is now known (so SWAR can be used safely)
      mulle_buffer_add_byte( buffer, '%');
      p         += 2;
      remaining -= 2;
      prefix     = scan_static_prefix_bounded( p, remaining);
   }
#else
   p = format;
   for(;;)
   {
      prefix = scan_static_prefix( p);
      if( prefix)
         mulle_buffer_add_bytes( buffer, p, prefix);
      p += prefix;

      if( ! *p)
         return( p);   // whole format was static
      if( ! escape_ok || p[ 1] != '%')
         return( p);   // a real conversion (or a stray '%') starts here

      // static escape "%%": emit a single '%' and scan the next chunk.
      mulle_buffer_add_byte( buffer, '%');
      p += 2;
   }
#endif
}

#endif


//
// parses all conversions of a format in a single forward pass, combining
// what used to be `number_of_conversions` (which counted and located the
// conversions) with the `parse_conversion_info` stage. Literal text is
// skipped once; each conversion is parsed once and the format is advanced
// past it. With more than STACKABLE_N conversions the stack buffers are
// migrated to (and grown within) the reusable thread-local storage.
//
// returns:  n  : success, number of conversions (may be 0)
//          -1  : an unsupported conversion character (errno=EINVAL)
//
static int   parse_all_conversions( struct mulle_sprintf_context *ctxt,
                                    const char *format,
                                    struct mulle_sprintf_conversion *table,
                                    int *max_arg)
{
   struct mulle_sprintf_formatconversioninfo   *infos;
   const char                                  **starts;
   const char                                  *p;
   int                                         arg;
   int                                         cap;
   int                                         grow;
   int                                         n;

   starts    = ctxt->startsBuf;
   infos     = ctxt->conversionBuf;
   cap       = STACKABLE_N;
   n         = 0;
   arg       = 1;
   *max_arg  = 0;

   for(;;)
   {
      // skip literal text up to the next '%' or the end of the format
      p = format;
      while( *p && *p != '%')
         ++p;
      if( ! *p)
      {
         ctxt->format_end = p;   // terminal NUL, for the trailing literal
         break;
      }

      if( n == cap)
      {
         const char   **os = starts;
         struct mulle_sprintf_formatconversioninfo   *oi = infos;

         grow = cap * 2;
         if( grow < n + 1)
            grow = n + 1;

         starts = space_for_starts( grow, &mulle_stdlib_allocator);
         infos  = space_for_infos( grow, &mulle_stdlib_allocator);
         if( os == ctxt->startsBuf)
         {  // migrate from the stack buffers
            memcpy( (void *) starts, os, n * sizeof( char *));
            memcpy( infos,  oi,  n * sizeof( struct mulle_sprintf_formatconversioninfo));
         }
         ctxt->starts = starts;
         ctxt->infos  = infos;
         cap          = grow;
      }

      arg = parse_conversion_info( p,
                                   &infos[ n],
                                   arg,
                                   max_arg,
                                   table->modifiers);
      if( arg <= 0)
      {
         // a '%' that never reaches a terminating conversion char is literal
         format = p + 1;
         continue;
      }

      if( determine_is_valid_conversion_character( table,
                                                   infos[ n].conversion) == -1)
      {
         errno = EINVAL;
         return( -1);
      }

      infos[ n].function = functions_for_conversion( table->jumps,
                                                     infos[ n].conversion);
      infos[ n].value_type = (unsigned char) determine_argument_type( &infos[ n]);

      starts[ n] = p;
      format     = p + infos[ n].length;
      ++n;
   }

   return( n);
}


static int  setup_context( struct mulle_sprintf_context *ctxt,
                            struct mulle_buffer *buffer,
                            const char *format,
                            struct mulle_sprintf_conversion *table)
{
   int                      argc;
   int                      max_arg;
   struct mulle_allocator   *allocator;

   allocator    = &mulle_stdlib_allocator;

   //
   // parse all conversions of the format in a single forward pass,
   // counting and parsing at the same time. grows past the stack
   // buffers to the reusable thread-local storage if needed.
   // on success ctxt->starts, ctxt->infos and ctxt->n are set
   //
   ctxt->starts = ctxt->startsBuf;
   ctxt->infos  = ctxt->conversionBuf;

   ctxt->n = parse_all_conversions( ctxt,
                                    format,
                                    table,
                                    &max_arg);
   if( ctxt->n <= 0)
      return( ctxt->n);

   argc = max_arg + 1;  // need one empty space in front
   if( argc <= STACKABLE_ARGUMENTS)
   {
      ctxt->argumentBuf.types  = ctxt->typesBuf;
      ctxt->argumentBuf.values = ctxt->valueBuf;
      ctxt->argumentBuf.size   = argc;
      ctxt->arguments          = &ctxt->argumentBuf;
   }
   else
      ctxt->arguments = space_for_arguments( argc, allocator);

   memset( ctxt->arguments->types,
           mulle_sprintf_int_argumenttype,
           sizeof( mulle_sprintf_argumenttype_t) * argc);

   //
   // now determine all types for all arguments used. The width and precision
   // are integer with is easy and already set above
   // (this may not touch all stack arguments!)
   //
   if( determine_all_conversion_argument_types( ctxt, table))
      return( -4);

   return( argc);
}


static int  context_print( struct mulle_sprintf_context *ctxt,
                           struct mulle_buffer *buffer,
                           const char *format,
                           int before)
{
   const char                                  *s;
   int                                         fail;
   int                                         i;
   ptrdiff_t                                   length;
   struct mulle_sprintf_formatconversioninfo   *info;

   // finally, finally oh so finally
   // print stuff

   // `before` is the buffer length captured before any static prefix was
   // copied, so %n and the return value count the prefix bytes too
   fail = 0;
   s    = format;
   for( i = 0; i < ctxt->n; i++)
   {
      // copy characters between conversions e.g. %d<characters>%d
      length = ctxt->starts[ i] - s;
      if( length)
      {
         mulle_buffer_add_bytes( buffer, s, length);
         s += length;
      }

      info = &ctxt->infos[ i];
      if( convert_argument( buffer, info, ctxt->arguments, before))
      {
         fail = 1;
      }
      s += info->length; // skip this part of the format
   }

   if( fail)
   {
      errno = EDOM;
      return( -1);
   }

   // trailing literal, with a known length (no strlen re-scan)
   length = ctxt->format_end - s;
   if( length)
      mulle_buffer_add_bytes( buffer, s, length);
   length = mulle_buffer_get_length( buffer) - before;

   if( mulle_buffer_has_overflown( buffer))
   {
      errno = ENOMEM;
      return( -1);
   }
   return( (int) length);
}


//
// horrible, we have to do it all again JUST because, it's impossible
// to extract argument #3 without having parsed the first two conversion
// specifiers completely...
// MAY RETURN
//       format (do a pointer comparison), means format _is_ output
//       buffer (""), means buffer contains output
//       NULL, an error occured
//
// may raise an exception if memory is full
//
int   _mulle_buffer_mvsprintf( struct mulle_buffer *buffer,
                               const char *format,
                               mulle_vararg_list va,
                               struct mulle_sprintf_conversion *table)
{
   struct mulle_sprintf_context   ctxt;
   int                            argc;
   int                            before;
   size_t                         len;

   //
   // copy a static prefix (literal text, maybe "%%" escapes) directly into
   // the buffer; only the remainder needs the full conversion treatment.
   // `before` is captured before the copy so that %n and the return value
   // still count the prefix bytes
   //
   before = (int) mulle_buffer_get_length( buffer);
#if MULLE_SPRINTF_PREFIX_OPTIMIZED
   format = copy_static_prefix( buffer, format, table);
#endif

   // now grab values from all arguments
   // there is no arg #0
   //
   argc = setup_context( &ctxt, buffer, format, table);
   if( argc < 0)
      return( argc);

   if( ! argc)
   {
      len = ctxt.format_end - format;
      mulle_buffer_add_bytes( buffer, format, len);  // we don't add a 0 byte
      if( mulle_buffer_has_overflown( buffer))
      {
         errno = ENOMEM;
         return( -1);
      }
      return( (int) mulle_buffer_get_length( buffer) - before);
   }

   mulle_mvsprintf_set_values( ctxt.arguments->values, ctxt.arguments->types, argc, va);

   return( context_print( &ctxt, buffer, format, before));
}



int   mulle_buffer_mvsprintf( struct mulle_buffer *buffer,
                              const char *format,
                              mulle_vararg_list arguments)
{
   if( ! buffer || ! format)
   {
      errno = EINVAL;
      return( -1);
   }

   return( _mulle_buffer_mvsprintf( buffer,
                                    format,
                                    arguments,
                                    mulle_sprintf_get_defaultconversion()));
}


#pragma mark - va_list


int   _mulle_buffer_vsprintf( struct mulle_buffer *buffer,
                              const char *format,
                              va_list va,
                              struct mulle_sprintf_conversion *table)
{
   struct mulle_sprintf_context   ctxt;
   int                            argc;
   int                            before;
   size_t                         len;

   //
   // copy a static prefix (literal text, maybe "%%" escapes) directly into
   // the buffer; only the remainder needs the full conversion treatment.
   // `before` is captured before the copy so that %n and the return value
   // still count the prefix bytes
   //
   before = (int) mulle_buffer_get_length( buffer);
#if MULLE_SPRINTF_PREFIX_OPTIMIZED
   format = copy_static_prefix( buffer, format, table);
#endif

   // now grab values from all arguments
   // there is no arg #0
   //
   argc = setup_context( &ctxt, buffer, format, table);
   if( argc < 0)
      return( argc);

   if( ! argc)
   {
      len = ctxt.format_end - format;
      mulle_buffer_add_bytes( buffer, format, len);
      if( mulle_buffer_has_overflown( buffer))
      {
         errno = ENOMEM;
         return( -1);
      }
      // we don't add a null byte (because this makes multiple vsprintfs painful)
      return( (int) mulle_buffer_get_length( buffer) - before);
   }

   mulle_vsprintf_set_values( ctxt.arguments->values, ctxt.arguments->types, argc, va);

   return( context_print( &ctxt, buffer, format, before));
}



int   mulle_buffer_vsprintf( struct mulle_buffer *buffer, const char *format, va_list args)
{
   if( ! buffer || ! format)
   {
      errno = EINVAL;
      return( -1);
   }
   return( _mulle_buffer_vsprintf( buffer,
                                   format,
                                   args,
                                   mulle_sprintf_get_defaultconversion()));
}


#pragma mark - stream print

int   mulle_buffer_sprintf( struct mulle_buffer *buffer, const char *format, ...)
{
   va_list   args;
   int       rval;

   if( ! buffer || ! format)
   {
      errno = EINVAL;
      return( -1);
   }

   va_start( args, format );
   rval = _mulle_buffer_vsprintf( buffer,
                                  format,
                                  args,
                                  mulle_sprintf_get_defaultconversion());
   va_end( args);

   return( rval);
}


#pragma mark - "C" <stdio> like interface

// these guarantee zero termination of the string

int   mulle_vsnprintf( char *buf, size_t size, const char *format, va_list va)
{
   struct mulle_buffer   buffer;
   int                   truncated;
   int                   rval;

   if( ! buf || ! size)
   {
      errno = EINVAL;
      return( -1);
   }

   mulle_buffer_init_inflexible_with_static_bytes( &buffer, buf, size);
   {
      rval      = _mulle_buffer_vsprintf( &buffer,
                                          format,
                                          va,
                                          mulle_sprintf_get_defaultconversion());
      truncated = mulle_buffer_make_string( &buffer);
      if( truncated)
      {
         errno = ENOMEM;
         rval  = -1;
      }
   }
   mulle_buffer_done( &buffer);

   return( rval);
}


int   mulle_mvsnprintf( char *buf,
                        size_t size,
                        const char *format,
                        mulle_vararg_list arguments)
{
   struct mulle_buffer   buffer;
   int                   truncated;
   int                   rval;

   if( ! buf || ! size)
   {
      errno = EINVAL;
      return( -1);
   }

   mulle_buffer_init_inflexible_with_static_bytes( &buffer, buf, size);
   {
      rval      = _mulle_buffer_mvsprintf( &buffer,
                                           format,
                                           arguments,
                                           mulle_sprintf_get_defaultconversion());
      truncated = mulle_buffer_make_string( &buffer);
      if( truncated)
      {
         errno = ENOMEM;
         rval  = -1;
      }
   }
   mulle_buffer_done( &buffer);

   return( rval);
}


int   mulle_snprintf( char *buf, size_t size, const char *format, ...)
{
   va_list   args;
   int       rval;

   va_start( args, format );
   rval = mulle_vsnprintf( buf, size, format, args);
   va_end( args);

   return( rval);
}


int   mulle_sprintf( char *buf, const char *format, ...)
{
   va_list   args;
   int       rval;

   va_start( args, format );
   rval = mulle_vsnprintf( buf, INT_MAX, format, args);
   va_end( args);

   return( rval);
}


int   mulle_vasprintf( char **strp, const char *format, va_list va)
{
   int    rval;
   char   *s;

   if( ! strp)
   {
      errno = EINVAL;
      return( -1);
   }

   mulle_buffer_do_string( buffer, NULL, s)
   {
      rval = _mulle_buffer_vsprintf( buffer,
                                     format,
                                     va,
                                     mulle_sprintf_get_defaultconversion());
   }
   *strp = s;

   return( rval);
}


int   mulle_mvasprintf( char **strp, const char *format, mulle_vararg_list arguments)
{
   int    rval;
   char   *s;

   if( ! strp)
   {
      errno = EINVAL;
      return( -1);
   }

   mulle_buffer_do_string( buffer, NULL, s)
   {
      rval = _mulle_buffer_mvsprintf( buffer,
                                      format,
                                      arguments,
                                      mulle_sprintf_get_defaultconversion());
   }
   *strp = s;

   return( rval);
}


int   mulle_asprintf( char **strp, const char *format, ...)
{
   va_list   args;
   int       rval;

   va_start( args, format);
   rval = mulle_vasprintf( strp, format, args);
   va_end( args);

   return( rval);
}


int   mulle_allocator_vasprintf( struct mulle_allocator *allocator,
                                 char **strp,
                                 const char *format,
                                 va_list va)
{
   int    rval;
   char   *s;

   if( ! strp)
   {
      errno = EINVAL;
      return( -1);
   }

   mulle_buffer_do_string( buffer, allocator, s)
   {
      rval = _mulle_buffer_vsprintf( buffer,
                                     format,
                                     va,
                                     mulle_sprintf_get_defaultconversion());
   }
   *strp = s;

   return( rval);
}


int   mulle_allocator_mvasprintf( struct mulle_allocator *allocator,
                                  char **strp,
                                  const char *format,
                                  mulle_vararg_list arguments)
{
   int    rval;
   char   *s;

   if( ! strp)
   {
      errno = EINVAL;
      return( -1);
   }

   mulle_buffer_do_string( buffer, allocator, s)
   {
      rval = _mulle_buffer_mvsprintf( buffer,
                                      format,
                                      arguments,
                                      mulle_sprintf_get_defaultconversion());
   }
   *strp = s;

   return( rval);
}


int   mulle_allocator_asprintf( struct mulle_allocator *allocator,
                                char **strp,
                                const char *format, ...)
{
   va_list   args;
   int       rval;

   va_start( args, format);
   rval = mulle_allocator_vasprintf( allocator, strp, format, args);
   va_end( args);

   return( rval);
}
