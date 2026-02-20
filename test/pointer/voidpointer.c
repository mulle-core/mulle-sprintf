#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>



static void   simple_pointer_test()
{
   struct mulle_buffer   *buffer;
   void                  *pointer;

   mulle_buffer_do( buffer)
   {
      mulle_buffer_sprintf( buffer, "%p", (void *) 0x1234FEAD); // some pointer
      sscanf( mulle_buffer_get_string( buffer), "%p", &pointer);

      if( pointer != (void *) 0x1234FEAD)
      {
         fprintf( stderr, "pointer mismatch\n");
         exit( 1);
      }

      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), mulle_buffer_get_string( buffer));
   }
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

