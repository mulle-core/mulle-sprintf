### 4.1.1




* format the most negative value of signed types (e.g. ``LLONG_MIN`,` ``INT64_MIN`)` correctly instead of relying on signed integer overflow
* update the ``mulle_sprintf_do`` / ``mulle_vsprintf_do`` macros for the updated mulle-buffer struct layout

## 4.1.0





* benchmarks time with ``mulle_timeinterval_now_monotonic()`` instead of ``clock_gettime``
* fix escaped `%%d` format strings so benchmark tests actually interpolate the value
* add a ``MULLE_TEST_VALGRIND`` build flag that lowers benchmark iterations for valgrind runs
* make `%n` static-prefix tests pass the format string as a fixed argument, fixing portability of the variadic call


feature: add SWAR prefix scan tuning knobs and SPRINTF-TUNING notes






* standardized BSD-3-Clause license header across all .c and .h files
* corrected copyright year from 2011 to 2018 in existing headers
* added API documentation link to README
