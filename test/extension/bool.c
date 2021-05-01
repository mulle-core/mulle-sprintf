#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>


static mulle_sprintf_argumenttype_t  get_bool_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   return( mulle_sprintf_int_argumenttype);
}


static int   _bool_conversion( struct mulle_buffer *buffer,
                               struct mulle_sprintf_formatconversioninfo *info,
                               struct mulle_sprintf_argumentarray *arguments,
                               int argc)
{
   union mulle_sprintf_argumentvalue   v;

   v = arguments->values[ argc];

   mulle_buffer_add_string( buffer, v.i ? "YES" : "NO");
   return( 0);
}


static struct mulle_sprintf_function   bool_functions =
{
   get_bool_argumenttype,
   _bool_conversion
};


static void  register_bool_sprintf_functions( void)
{
   mulle_sprintf_register_functions( NULL, &bool_functions, 'b');
}


static void   test()
{
   struct mulle_flushablebuffer   buffer;
   auto char                      storage[ 256];

   mulle_flushablebuffer_init( &buffer,
                               storage,
                               sizeof( storage),
                               (mulle_flushablebuffer_flusher *) fwrite,
                               stdout);

   mulle_sprintf( (struct mulle_buffer *) &buffer, "%b %b %b\n", 1, 0, -1);

   mulle_flushablebuffer_done( &buffer);
}




int  main()
{
   mulle_testallocator_set_tracelevel( 1);
   mulle_testallocator_initialize();
   mulle_default_allocator = mulle_testallocator;

   register_bool_sprintf_functions();
   test();

   mulle_sprintf_free_storage();
   mulle_testallocator_reset();
}

