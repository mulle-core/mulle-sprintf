/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintf.m is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK.
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#ifndef mulle_sprintf_h__
#define mulle_sprintf_h__

#include "mulle_sprintf_function.h"
#include <mulle_vararg/mulle_vararg.h>
#include <mulle_thread/mulle_thread.h>


#define MULLE_SPRINTF_VERSION  ((0 << 20) | (6 << 8) | 0)


#ifndef MULLE_SPRINTF_EXTERN_GLOBAL
# define MULLE_SPRINTF_EXTERN_GLOBAL    MULLE_C_EXTERN_GLOBAL
#endif



struct mulle_buffer;
struct mulle_sprintf_conversion;


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


static inline void    mulle_sprintf_free_storage( void)
{
   (*mulle_sprintf_get_config()->free_storage)();
}


#if MULLE_ALLOCATOR_VERSION < ((1 << 20) | (5 << 8) | 0)
# error "mulle_allocator is too old"
#endif
#if MULLE_BUFFER_VERSION < ((0 << 20) | (2 << 8) | 0)
# error "mulle_buffer is too old"
#endif

#endif
