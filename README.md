# mulle-sprintf

an extensible variant of sprintf written in C (C11). Because floating point is
hard, FP conversion is handed down to `sprintf`.

The extensibility is used in **MulleObjCFoundation** to add the '@' conversion
character for object conversion.

mulle-sprintf can handle **varargs** and **mulle_vararg** style variable
arguments.



## Author

[Nat!](//www.mulle-kybernetik.com/weblog) for
[Mulle kybernetiK](//www.mulle-kybernetik.com) and
[Codeon GmbH](//www.codeon.de)
