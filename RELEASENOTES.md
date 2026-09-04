### 4.1.0

* const-qualify input-only pointer parameters
* add SWAR prefix scan tuning knobs and SPRINTF-TUNING notes
* various performance tuning and improvements
* add missing copyright and license headers to source files

### 4.0.1



* -0.0 is no longer normalized to 0.0 in %e, %f, %g, %a conversions
* output now matches C standard library convention for negative zero
* -nan sign bit continues to be normalized per IEEE 754 semantics