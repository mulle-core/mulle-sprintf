### 4.0.1



* -0.0 is no longer normalized to 0.0 in %e, %f, %g, %a conversions
* output now matches C standard library convention for negative zero
* -nan sign bit continues to be normalized per IEEE 754 semantics
