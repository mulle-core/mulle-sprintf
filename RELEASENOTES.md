# 4.0.0



feature: standardize %c/%C and %s/%S UTF conversion handling

* %C and %lC consistently use UTF-32; %S and %lS consistently use UTF-32 across platforms
* %c and %s use char/UTF-8 by default; 'l'/'h' modifiers select wide/UTF variants when `HAVE_MULLE_UTF` is enabled



  - Windows: %p -> %zx (Windows sscanf doesn't parse lowercase hex)
  - All platforms: %f/%e/%g -> %lf/%le/%lg (scanf needs 'l' for double)

* %p is now lowercase hex


* prevent Windows crashes when formatting ``wchar_t` *` wide strings with `%ls`
* **BREAKING**: `%S` now takes ``utf32_t` *` by default and `%lS` now takes ``wchar_t` *` (platform-dependent), matching standard wide-string expectations


* fix utf32 problem for %C on windows
