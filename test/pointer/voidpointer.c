#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>



static void   simple_pointer_test()
{
   void                  *pointer;

   mulle_buffer_do( buffer)
   {
      mulle_buffer_sprintf( buffer, "%p", (void *) 0x1234FEAD); // some pointer
      mulle_sscanf( mulle_buffer_get_string( buffer), "%p", &pointer);

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


   simple_pointer_test();

   mulle_sprintf_free_storage();

   return( 0);
}

