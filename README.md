# mulle-sprintf

an extensible variant of sprintf written in C (C11). Because floating point is 
hard, FP conversion is handed down to sprintf.

The extensibility is used in **MulleObjCFoundation** to add the '@' conversion
character for object conversion.

mulle-sprintf can handle **varargs** and **mulle_vararg** style variable
arguments.


## Dependencies

* [mulle-allocator](//www.mulle-kybernetik.com/repositories/mulle-allocator)
* [mulle-thread](//www.mulle-kybernetik.com/repositories/mulle-thread)
* [mulle-vararg](//www.mulle-kybernetik.com/repositories/mulle-vararg)
* xcodebuild for OS X
* cmake 3.0 for other Unixes

