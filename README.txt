Description.
small library for converting utf16 strings to utf8 and back.

Author.
Copyright (C) 2018 Michael M. Builov, https://github.com/mbuilov/libutf16

License.
Apache License v2.0, see LICENSE.TXT.

Optional dependencies.
"sal_defs.h" from https://github.com/mbuilov/cmn_headers



Building.
You need any c99 compiler,

for example gcc:
gcc -g -O2 -I. -c ./src/utf16_to_utf8.c
gcc -g -O2 -I. -c ./src/utf8_to_utf16.c
ar -crs libutf16.a ./utf16_to_utf8.o ./utf8_to_utf16.o

or MSVC:
cl /O2 /I. /c .\src\utf16_to_utf8.c
cl /O2 /I. /c .\src\utf8_to_utf16.c
lib /out:utf16.a .\utf16_to_utf8.obj .\utf8_to_utf16.obj



May build with source annontations (restricted pointers, non-null attributes, etc.)

gcc:
gcc -g -O2 -include ../cmn_headers/sal_defs.h -I. -c ./src/utf16_to_utf8.c
gcc -g -O2 -include ../cmn_headers/sal_defs.h -I. -c ./src/utf8_to_utf16.c
ar -crs libutf16.a ./utf16_to_utf8.o ./utf8_to_utf16.o

MSVC:
cl /O2 /FI..\cmn_headers\sal_defs.h /I. /c .\src\utf16_to_utf8.c
cl /O2 /FI..\cmn_headers\sal_defs.h /I. /c .\src\utf8_to_utf16.c
lib /out:utf16.a .\utf16_to_utf8.obj .\utf8_to_utf16.obj
