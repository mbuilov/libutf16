Description.
small library for converting utf16 strings to utf8 and back.

Dependencies.
https://github.com/mbuilov/cmn_headers

Building.
You need any c99 compiler,

for example gcc:
gcc -I../cmn_headers -I. -c ./src/utf16_to_utf8.c
gcc -I../cmn_headers -I. -c ./src/utf8_to_utf16.c
ar -crs libutf16.a ./utf16_to_utf8.o ./utf8_to_utf16.o

or MSVC:
cl /c /Dinline=__inline /I..\cmn_headers /I. .\src\utf16_to_utf8.c
cl /c /Dinline=__inline /I..\cmn_headers /I. .\src\utf8_to_utf16.c
lib /out:utf16.a .\utf16_to_utf8.obj .\utf8_to_utf16.obj
