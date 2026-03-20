//
//  mulle_sscanf.c
//  mulle-sprintf
//
//  Created by Nat!
//  Copyright (c) 2026 Nat! - Mulle kybernetiK.
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

#include "mulle-sscanf.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>


static size_t   mulle_sscanf_compute_rewritten_length( char const *format,
                                                        int *p_extra)
{
   char const   *src;
   int          extra;
   int          in_percent;
   int          has_modifier;

   extra = 0;
   src   = format;

   while( *src)
   {
      if( *src == '%')
      {
         src++;
         if( *src == '%')
         {
            src++;
            continue;
         }

         in_percent   = 1;
         has_modifier = 0;

         while( *src && in_percent)
         {
            if( *src == '*' || isdigit( (unsigned char) *src))
            {
               src++;
               continue;
            }

            if( *src == 'h' || *src == 'L' || *src == 'j' ||
                *src == 'z' || *src == 't' || *src == 'q')
            {
               has_modifier = 1;
               src++;
               continue;
            }

            if( *src == 'l')
            {
               has_modifier = 1;
               src++;
               continue;
            }

#ifdef _WIN32
            if( *src == 'p')
               extra += 1;
#endif

            if( ! has_modifier && (*src == 'f' || *src == 'e' || *src == 'g'))
               extra += 1;

            in_percent = 0;
            if( *src)
               src++;
         }
      }
      else
         src++;
   }

   *p_extra = extra;
   return( strlen( format) + extra + 1);
}


static void   mulle_sscanf_rewrite_format( char const *format,
                                            char *new_format)
{
   char const   *src;
   char         *dst;
   int          in_percent;
   int          has_modifier;

   dst          = new_format;
   src          = format;
   in_percent   = 0;
   has_modifier = 0;

   while( *src)
   {
      if( *src == '%')
      {
         *dst++ = *src++;
         if( *src == '%')
         {
            *dst++       = *src++;
            in_percent   = 0;
            has_modifier = 0;
            continue;
         }
         in_percent   = 1;
         has_modifier = 0;
      }
      else if( in_percent)
      {
         if( *src == '*' || isdigit( (unsigned char) *src))
         {
            *dst++ = *src++;
         }
         else if( *src == 'h' || *src == 'L' || *src == 'j' ||
                  *src == 'z' || *src == 't' || *src == 'q' || *src == 'l')
         {
            has_modifier = 1;
            *dst++       = *src++;
         }
         else
         {
#ifdef _WIN32
            if( *src == 'p')
            {
               *dst++ = 'z';
               *dst++ = 'x';
               src++;
               in_percent   = 0;
               has_modifier = 0;
               continue;
            }
#endif
            if( ! has_modifier && (*src == 'f' || *src == 'e' || *src == 'g'))
            {
               *dst++ = 'l';
            }
            *dst++       = *src++;
            in_percent   = 0;
            has_modifier = 0;
         }
      }
      else
      {
         *dst++ = *src++;
      }
   }
   *dst = '\0';
}


int   mulle_vsscanf( char const *str, char const *format, va_list args)
{
   size_t   len;
   int      extra;
   int      rval;

   len = mulle_sscanf_compute_rewritten_length( format, &extra);

   if( ! extra)
      return( vsscanf( str, format, args));

   mulle_alloca_do( rewritten, char, len)
   {
      mulle_sscanf_rewrite_format( format, rewritten);
      rval = vsscanf( str, rewritten, args);
   }

   return( rval);
}


int   mulle_sscanf( char const *str, char const *format, ...)
{
   va_list   args;
   int       rval;

   va_start( args, format);
   rval = mulle_vsscanf( str, format, args);
   va_end( args);

   return( rval);
}
