#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>



static void   simple_pointer_test()
{
   struct mulle_buffer   *buffer;
   void                  *pointer;

   buffer = mulle_buffer_create( NULL);

   mulle_buffer_sprintf( buffer, "%p", (void *) 0x12345678); // some pointer
   mulle_buffer_add_byte( buffer, 0);
   sscanf( mulle_buffer_get_bytes( buffer), "%p", &pointer);
   mulle_buffer_reset( buffer);

   if( pointer != (void *) 0x12345678)
      printf( "pointer mismatch\n");

   mulle_buffer_destroy( buffer);
}


int  main()
{
   mulle_testallocator_set_tracelevel( 1);
   mulle_testallocator_initialize();

   mulle_default_allocator = mulle_testallocator;

   simple_pointer_test();

   mulle_sprintf_free_storage();
   mulle_testallocator_reset();

   return( 0);
}

