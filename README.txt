Description.
small library for converting utf16 strings to utf8 and back.

Author.
Copyright (C) 2018-2020 Michael M. Builov, https://github.com/mbuilov/libutf16

License.
Apache License v2.0, see LICENSE.TXT.

Optional dependencies.
"sal_defs.h" from https://github.com/mbuilov/cmn_headers


Notes.
1) The library do not performs endianess conversion of utf16-characters, assuming
  UTF-16LE - on Little-endian platforms and
  UTF-16BE - on Big-endian platforms.
2) Byte Order Marks (BOM) are not supported.


Building.
You need any c99 compiler,

for example gcc:
gcc -g -O2 -I. -c -Wall -Wextra ./src/utf16_to_utf8.c
gcc -g -O2 -I. -c -Wall -Wextra ./src/utf8_to_utf16.c
gcc -g -O2 -I. -c -Wall -Wextra ./src/utf8_to_utf16_one.c
gcc -g -O2 -I. -c -Wall -Wextra ./src/utf16_to_utf8_one.c
gcc -g -O2 -I. -c -Wall -Wextra ./src/utf8_cstd.c
ar -crs libutf16.a ./utf16_to_utf8.o ./utf8_to_utf16.o ./utf8_to_utf16_one.o ./utf16_to_utf8_one.o ./utf8_cstd.o

or MSVC:
cl /O2 /I. /c /Wall .\src\utf16_to_utf8.c
cl /O2 /I. /c /Wall .\src\utf8_to_utf16.c
cl /O2 /I. /c /Wall .\src\utf8_to_utf16_one.c
cl /O2 /I. /c /Wall .\src\utf16_to_utf8_one.c
cl /O2 /I. /c /Wall .\src\utf8_cstd.c
lib /out:utf16.a .\utf16_to_utf8.obj .\utf8_to_utf16.obj .\utf8_to_utf16_one.obj .\utf16_to_utf8_one.obj .\utf8_cstd.obj



Also, the library can be built with source annotations (restricted pointers, non-null attributes, etc.)

gcc:
gcc -g -O2 -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra ./src/utf16_to_utf8.c
gcc -g -O2 -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra ./src/utf8_to_utf16.c
gcc -g -O2 -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra ./src/utf8_to_utf16_one.c
gcc -g -O2 -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra ./src/utf16_to_utf8_one.c
gcc -g -O2 -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra ./src/utf8_cstd.c
ar -crs libutf16.a ./utf16_to_utf8.o ./utf8_to_utf16.o ./utf8_to_utf16_one.o ./utf16_to_utf8_one.o ./utf8_cstd.o

MSVC:
cl /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall /wd4464 /analyze .\src\utf16_to_utf8.c
cl /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall /wd4464 /analyze .\src\utf8_to_utf16.c
cl /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall /wd4464 /analyze .\src\utf8_to_utf16_one.c
cl /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall /wd4464 /analyze .\src\utf16_to_utf8_one.c
cl /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall /wd4464 /analyze .\src\utf8_cstd.c
lib /out:utf16.a .\utf16_to_utf8.obj .\utf8_to_utf16.obj .\utf8_to_utf16_one.obj .\utf16_to_utf8_one.obj .\utf8_cstd.obj
