//
//  mulle_sprintf.c
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

#include "mulle-sprintf.h"
#include "mulle-sprintf-function.h"

#include "include-private.h"


#ifndef HAVE_SPRINTF_BOOL
# define HAVE_SPRINTF_BOOL  1
#endif

#define STACKABLE_N     16

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
//
struct mulle_sprintf_malloc_storage
{
   char                                        **starts;
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
   mulle_allocator_free( storage->allocator, storage->starts);
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
      storage->starts   = mulle_allocator_realloc( allocator, storage->starts, size);
      storage->s_starts = size;
   }
   return( storage->starts);
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
   jump_determine_argument_type( mulle_sprintf_vector_t  jumptable,
                                 struct mulle_sprintf_formatconversioninfo *info)
{
   struct mulle_sprintf_function  *functions;

   functions = functions_for_conversion( jumptable, info->conversion);
   if( ! functions)
      return( (mulle_sprintf_argumenttype_t) -1);

   return( (*functions->determine_argument_type)( info));
}


static inline int
   jump_convert_argument( mulle_sprintf_vector_t  jumptable,
                          struct mulle_buffer *buffer,
                          struct mulle_sprintf_formatconversioninfo *info,
                          struct mulle_sprintf_argumentarray *arguments,
                          int i)
{
   struct mulle_sprintf_function  *functions;

   functions = functions_for_conversion( jumptable, info->conversion);
   if( ! functions)
      return( -1);

   return( (*functions->convert_argument)( buffer, info, arguments, i));
}


static inline mulle_sprintf_argumenttype_t
   determine_argument_type( mulle_sprintf_vector_t  jumptable,
                            struct mulle_sprintf_formatconversioninfo *info)
{
   // need to code this (possibly :))
   if( info->modifier[ 0] == 'v' ||
       info->modifier[ 1] == 'v' ||
       info->modifier[ 2] == 'v')
      return( mulle_sprintf_vector_argumenttype);

   return( jump_determine_argument_type( jumptable, info));
}


static inline void
   update_width_and_precision( struct mulle_sprintf_formatconversioninfo *info,
                               struct mulle_sprintf_argumentarray *arguments,
                               int *argc)
{
   if( info->memory.width_is_argument)
   {
      if( info->memory.width_is_indexed_argument)
         *argc = info->argv_index[ 1];
      else
         ++*argc;

      info->width = arguments->values[ *argc].i;
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

   if( info->memory.precision_is_argument)
   {
      if( info->memory.precision_is_indexed_argument)
         *argc = info->argv_index[ 2];
      else
         ++*argc;

      info->precision = arguments->values[ *argc].i;
      if( info->precision < 0)
         info->precision = - info->precision;
   }
}


static inline int
   convert_argument( mulle_sprintf_vector_t jumptable,
                     struct mulle_buffer *buffer,
                     struct mulle_sprintf_formatconversioninfo *info,
                     struct mulle_sprintf_argumentarray *arguments,
                     int *arg,
                     int before)
{
   if( info->modifier[ 0] == 'v' ||
       info->modifier[ 1] == 'v' ||
       info->modifier[ 2] == 'v')
   {
      mulle_buffer_add_string( buffer, "<vector unsupported>");
      return( -1);
   }

   if( info->memory.argument_index_found)
      *arg = info->argv_index[ 0];

   update_width_and_precision( info, arguments, arg);

   if( info->memory.argument_index_found)
      *arg = info->argv_index[ 0];
   else
      ++*arg;

   //
   // to do, extract by type here and then omit arguments and i from
   // conversion call
   // there's just two things we can't vector
   //
   info->mystery = (void *) (intptr_t) before;  // for return conversion
   return( jump_convert_argument( jumptable, buffer, info, arguments, *arg));
}


// start is "after" %
static int   number_of_conversions( char *format,
                                    char **starts,
                                    char **sentinel,
                                    char **remainder,
                                    struct mulle_sprintf_conversion *table)
{
   int    n;
   char   c;

   *remainder = NULL;

   n = 0;
   while( c = *format)
   {
      if( c == '%')
      {
         if( starts)
         {
            if( starts == sentinel)
            {
               *remainder = format;
               starts     = NULL;
            }
            else
               *starts++ = format;
         }

         while( c = *++format)
         {
            //
            // TODO: this does not catch mixups of %tbd vs (correct) %btd
            //       therefore the error checking is superflous and needs
            //       to be done later
            switch( determine_is_valid_conversion_character( table, c))
            {
            case 0  : continue;        // not yet, probably a modifier
            case 1  : ++n; goto next;  // yes
            case -1 : errno = EINVAL; return( -1); // if we don't support now
                                                   // we might in the future
                                                   // printing would be IMO bad
            }
         }
      }
next:
      ++format;
   }

   return( n);
}


typedef struct
{
   char    *start;
   char    *curr;
   char    *memo;
   char    *sentinel;

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
   char     buf[ 63 + 1];
   size_t   len;

   len = parser->curr - parser->memo;
   if( ! len)
      return( 0);  // it's OK!
   if( len >= sizeof( buf))
      return( -1);

   memcpy( buf, parser->memo, len);
   buf[ len] = 0;

   return( atoi( buf));
}


//
// returns: number of arguments it needs
//
static inline int
   parse_conversion_info( char *format,
                          struct mulle_sprintf_formatconversioninfo *info,
                          int arg)
{
   format_conversion_parser  parser;
   char           c;
   int            value;

   memset( info, 0, sizeof( struct mulle_sprintf_formatconversioninfo));
   parser.memo     = NULL;
   parser.curr     = format;
   parser.sentinel = &format[ 127];  // %2147483647$0#- +\[,;:_]2147483647.2147483647hllX"

   parser.memo  = NULL;
   parser.state = state_begin;

   parser.modifier_index = 0;

   while( c = *++parser.curr)
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

               info->argv_index[ 1] = value;
               parser.memo          = NULL;
               parser.state         = state_precision;
               arg                  = value;
               continue;
            }

            if( ! info->memory.width_is_argument)
            {
               info->width = positive_int_value_from_memo( &parser, info);
               if( info->width < 0)
                  return( -1);
            }
            else
            {
               if( parser.memo[ -1] != '*')
                  return( -1);
               ++arg;     // consume one
            }
         }
         parser.state = state_precision;
         parser.memo = NULL;

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
               if( value == 0)
                  return( -1);

               info->memory.precision_is_indexed_argument = 1;
               info->argv_index[ 2]                       = value;
               parser.state                               = state_modifier;
               arg                                        = value;
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
               ++arg;     // consume one
            }
               // parser.memo = NULL;
         }
         parser.state = state_modifier;  // fall thru

      case state_modifier :
         // TODO: no _real_ need to hardcode this or ?
         switch( c)
         {
         case 'h' :
         case 'l' :
         case 'j' :
         case 't' :
         case 'z' :
         case 'q' :
         case 'L' :
            if( parser.modifier_index >= 3)
               return( -1);

            info->modifier[ parser.modifier_index++] = c;
            continue;
         }
         parser.state = state_conversion; // fall thru

      case state_conversion :
         info->conversion = c;
         info->length     = (int) ((parser.curr - format) + 1);
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
   char                                        **starts;
   char                                        *startsBuf[ STACKABLE_N];
   struct mulle_sprintf_argumentarray          *arguments;
   struct mulle_sprintf_argumentarray          argumentBuf;
   struct mulle_sprintf_formatconversioninfo   *infos;
   struct mulle_sprintf_formatconversioninfo   conversionBuf[ STACKABLE_N];  // this is the biggy
   union mulle_sprintf_argumentvalue           valueBuf[ STACKABLE_N];
   unsigned char                               typesBuf[ STACKABLE_N];
   int                                         before;
   int                                         n;
};


static int
   determine_all_conversion_argument_types( struct mulle_sprintf_context *ctxt,
                                            struct mulle_sprintf_conversion *table)
{
   struct mulle_sprintf_formatconversioninfo   *info;
   int                                         i;
   int                                         arg;

   arg = 0;

   for( i = 0; i < ctxt->n; i++)
   {
      info = &ctxt->infos[ i];

      if( info->memory.argument_index_found)
         arg = info->argv_index[ 0];
      else
      {
         if( info->memory.width_is_argument && ! info->memory.width_is_indexed_argument)
            ++arg;

         if( info->memory.precision_is_argument && ! info->memory.precision_is_indexed_argument)
            ++arg;

         ++arg;
      }

      ctxt->arguments->types[ arg] = determine_argument_type( table->jumps, info);
      if( ctxt->arguments->types[ arg] == (mulle_sprintf_argumenttype_t) - 1)
         return( -4);

      // looks so wrong, but is sprintf compatible
      if( info->memory.width_is_argument && info->memory.width_is_indexed_argument)
         arg = info->argv_index[ 1];

      if( info->memory.precision_is_argument && info->memory.precision_is_indexed_argument)
         arg = info->argv_index[ 2];
   }

   return( 0);
}



static int  setup_context( struct mulle_sprintf_context *ctxt,
                           struct mulle_buffer *buffer,
                           char *format,
                           struct mulle_sprintf_conversion *table)
{
   char                     *remaining_format;
   int                      arg;
   int                      argc;
   int                      i;
   int                      max_arg;
   struct mulle_allocator   *allocator;

   ctxt->n = number_of_conversions( format,
                                    ctxt->startsBuf,
                                    &ctxt->startsBuf[ STACKABLE_N],
                                    &remaining_format,
                                    table);
   if( ctxt->n <= 0)
      return( ctxt->n);  // if we have a malformed % we bail usual printf
                         // just prints, but I don't like it

   allocator    = mulle_buffer_get_allocator( buffer);
   ctxt->before = (int) mulle_buffer_get_length( buffer);

   ctxt->starts = ctxt->startsBuf;
   ctxt->infos  = ctxt->conversionBuf;
   if( ctxt->n >= STACKABLE_N)
   {
      ctxt->starts = space_for_starts( ctxt->n, allocator);
      ctxt->infos  = space_for_infos( ctxt->n, allocator);
      memcpy( ctxt->starts, ctxt->startsBuf, STACKABLE_N * sizeof( char *));


      number_of_conversions( remaining_format,
                             &ctxt->starts[ STACKABLE_N],
                             &ctxt->starts[ ctxt->n],
                             &remaining_format,
                             table);
   }

   //
   // now parse the conversion specifiers into their "infos"
   // at the same time, we calculate the
   // max index of arguments consumed from stack
   // args are indexed from 1 to argc

   arg     = 0;
   max_arg = 0;

   for( i = 0; i < ctxt->n; i++)
   {
      arg = parse_conversion_info( ctxt->starts[ i], &ctxt->infos[ i], arg);
      if( arg <= 0)
         return( -4);

      if( arg > max_arg)
         max_arg = arg;
   }

   argc = max_arg + 1;
   if( argc <= STACKABLE_N)
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
                           char *format,
                           struct mulle_sprintf_conversion *table)
{
   char                                        *s;
   int                                         arg;
   int                                         fail;
   int                                         i;
   ptrdiff_t                                   length;
   struct mulle_sprintf_formatconversioninfo   *info;

   // finally, finally oh so finally
   // print stuff

   fail = 0;
   s    = format;
   arg  = 0;
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
      if( convert_argument( table->jumps,
                            buffer,
                            info,
                            ctxt->arguments,
                            &arg,
                            ctxt->before))
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

   mulle_buffer_add_string( buffer, s);
   length = mulle_buffer_get_length( buffer) - ctxt->before;

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
                               char *format,
                               mulle_vararg_list va,
                               struct mulle_sprintf_conversion *table)
{
   struct mulle_sprintf_context   ctxt;
   int                            argc;
   size_t                         len;

   // now grab values from all arguments
   // there is no arg #0
   //
   argc = setup_context( &ctxt, buffer, format, table);
   if( argc < 0)
      return( argc);

   if( ! argc)
   {
      len = strlen( format);
      mulle_buffer_add_bytes( buffer, format, len);  // we don't add a 0 byte
      if( mulle_buffer_has_overflown( buffer))
      {
         errno = ENOMEM;
         return( -1);
      }
      return( (int) len);
   }

   mulle_mvsprintf_set_values( ctxt.arguments->values, ctxt.arguments->types, argc, va);

   return( context_print( &ctxt, buffer, format, table));
}



int   mulle_buffer_mvsprintf( struct mulle_buffer *buffer,
                              char *format,
                              mulle_vararg_list arguments)
{
   if( ! buffer || ! format)
   {
      errno = EINVAL;
      return( -1);
   }

   return( _mulle_buffer_mvsprintf( buffer, format, arguments, mulle_sprintf_get_defaultconversion()));
}


#pragma mark - va_list


int   _mulle_buffer_vsprintf( struct mulle_buffer *buffer,
                              char *format,
                              va_list va,
                              struct mulle_sprintf_conversion *table)
{
   struct mulle_sprintf_context   ctxt;
   int                            argc;
   size_t                         len;

   // now grab values from all arguments
   // there is no arg #0
   //
   argc = setup_context( &ctxt, buffer, format, table);
   if( argc < 0)
      return( argc);

   if( ! argc)
   {
      len = strlen( format);
      mulle_buffer_add_bytes( buffer, format, len);
      if( mulle_buffer_has_overflown( buffer))
      {
         errno = ENOMEM;
         return( -1);
      }
      // we don't add a null byte (because this makes multiple vsprintfs painful)
      return( (int) len);
   }

   mulle_vsprintf_set_values( ctxt.arguments->values, ctxt.arguments->types, argc, va);

   return( context_print( &ctxt, buffer, format, table));
}



int   mulle_buffer_vsprintf( struct mulle_buffer *buffer, char *format, va_list args)
{
   if( ! buffer || ! format)
   {
      errno = EINVAL;
      return( -1);
   }
   return( _mulle_buffer_vsprintf( buffer, format, args, mulle_sprintf_get_defaultconversion()));
}


#pragma mark - stream print

int   mulle_buffer_sprintf( struct mulle_buffer *buffer, char *format, ...)
{
   va_list   args;
   int       rval;

   if( ! buffer || ! format)
   {
      errno = EINVAL;
      return( -1);
   }

   va_start( args, format );
   rval = _mulle_buffer_vsprintf( buffer, format, args, mulle_sprintf_get_defaultconversion());
   va_end( args);
   return( rval);
}


#pragma mark - "C" <stdio> like interface

// these guarantee zero termination of the string

int   mulle_vsnprintf( char *buf, size_t size, char *format, va_list va)
{
   int                   rval;
   int                   truncated;
   struct mulle_buffer   buffer;

   if( ! buf || ! size)
   {
      errno = EINVAL;
      return( -1);
   }
   mulle_buffer_init_inflexible_with_static_bytes( &buffer, buf, size);

   rval = _mulle_buffer_vsprintf( &buffer, format, va, mulle_sprintf_get_defaultconversion());

   truncated = mulle_buffer_make_string( &buffer);
   mulle_buffer_done( &buffer);

   if( truncated)
   {
      errno = ENOMEM;
      rval  = -1;
   }

   if( rval == -1)
      *buf = 0;
   return( rval);
}


int   mulle_mvsnprintf( char *buf, size_t size, char *format, mulle_vararg_list arguments)
{
   int                   rval;
   int                   truncated;
   struct mulle_buffer   buffer;

   if( ! buf || ! size)
   {
      errno = EINVAL;
      return( -1);
   }

   mulle_buffer_init_inflexible_with_static_bytes( &buffer, buf, size);

   rval = _mulle_buffer_mvsprintf( &buffer, format, arguments, mulle_sprintf_get_defaultconversion());

   truncated = mulle_buffer_make_string( &buffer);
   mulle_buffer_done( &buffer);

   if( truncated)
   {
      errno = ENOMEM;
      rval  = -1;
   }

   if( rval == -1)
      *buf = 0;

   return( rval);
}


int   mulle_snprintf( char *buf, size_t size, char *format, ...)
{
   va_list               args;
   int                   rval;

   va_start( args, format );
   rval = mulle_vsnprintf( buf, size, format, args);
   va_end( args);

   return( rval);
}


int   mulle_sprintf( char *buf, char *format, ...)
{
   va_list   args;
   int       rval;

   va_start( args, format );
   rval = mulle_vsnprintf( buf, INT_MAX, format, args);
   va_end( args);

   return( rval);
}


int   mulle_vasprintf( char **strp, char *format, va_list va)
{
   struct mulle_buffer   buffer;
   int                   rval;

   if( ! strp)
   {
      errno = EINVAL;
      return( -1);
   }

   mulle_buffer_init_with_capacity( &buffer, 256, NULL);

   rval = _mulle_buffer_vsprintf( &buffer, format, va, mulle_sprintf_get_defaultconversion());

   *strp = mulle_buffer_extract_string( &buffer);
   mulle_buffer_done( &buffer);

   return( rval);
}


int   mulle_mvasprintf( char **strp, char *format, mulle_vararg_list arguments)
{
   struct mulle_buffer   buffer;
   int                   rval;

   if( ! strp)
   {
      errno = EINVAL;
      return( -1);
   }

   mulle_buffer_init_with_capacity( &buffer, 256, NULL);

   rval = _mulle_buffer_mvsprintf( &buffer, format, arguments, mulle_sprintf_get_defaultconversion());

   *strp = mulle_buffer_extract_string( &buffer);
   mulle_buffer_done( &buffer);

   return( rval);
}


int   mulle_asprintf( char **strp, char *format, ...)
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
                                 char *format,
                                 va_list va)
{
   struct mulle_buffer   buffer;
   int                   rval;

   if( ! strp)
   {
      errno = EINVAL;
      return( -1);
   }

   mulle_buffer_init_with_capacity( &buffer, 256, allocator);

   rval = _mulle_buffer_vsprintf( &buffer, format, va, mulle_sprintf_get_defaultconversion());

   *strp = mulle_buffer_extract_string( &buffer);
   mulle_buffer_done( &buffer);

   return( rval);
}


int   mulle_allocator_mvasprintf( struct mulle_allocator *allocator,
                                  char **strp,
                                  char *format,
                                  mulle_vararg_list arguments)
{
   struct mulle_buffer   buffer;
   int                   rval;

   if( ! strp)
   {
      errno = EINVAL;
      return( -1);
   }

   mulle_buffer_init_with_capacity( &buffer, 256, allocator);

   rval = _mulle_buffer_mvsprintf( &buffer, format, arguments, mulle_sprintf_get_defaultconversion());

   *strp = mulle_buffer_extract_string( &buffer);
   mulle_buffer_done( &buffer);

   return( rval);
}


int   mulle_allocator_asprintf( struct mulle_allocator *allocator, char **strp, char *format, ...)
{
   va_list   args;
   int       rval;

   va_start( args, format);
   rval = mulle_allocator_vasprintf( allocator, strp, format, args);
   va_end( args);

   return( rval);
}
