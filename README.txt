Description.
small library for converting between utf8/utf16/utf32 strings.

Author.
Copyright (C) 2018-2022 Michael M. Builov, https://github.com/mbuilov/libutf16

License.
Apache License v2.0, see LICENSE.TXT.


Notes.
1) By default, library do not performs endianness conversion of utf16/utf32-characters, assuming
  UTF-16LE/UTF-32LE - on Little-endian platforms and
  UTF-16BE/UTF-32BE - on Big-endian platforms.
2) However, some encoding functions allow to swap bytes of utf16/utf32-characters while reading/writing to buffers.
3) Byte Order Marks (BOM) are not supported.


Building.
You need any c99 compiler,

for example gcc:
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf32_to_utf16.c    -o ./src/utf32_to_utf16.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                      -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32x_to_utf16.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                         -DSWAP_UTF16              ./src/utf32_to_utf16.c    -o ./src/utf32_to_utf16x.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                         -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32x_to_utf16x.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED                           ./src/utf32_to_utf16.c    -o ./src/utf32_to_utf16u.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED              -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32x_to_utf16u.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED -DSWAP_UTF16              ./src/utf32_to_utf16.c    -o ./src/utf32_to_utf16ux.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32x_to_utf16ux.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                               ./src/utf32_to_utf16.c    -o ./src/utf32u_to_utf16.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                  -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32ux_to_utf16.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                     -DSWAP_UTF16              ./src/utf32_to_utf16.c    -o ./src/utf32u_to_utf16x.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                     -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32ux_to_utf16x.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED                           ./src/utf32_to_utf16.c    -o ./src/utf32u_to_utf16u.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED              -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32ux_to_utf16u.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED -DSWAP_UTF16              ./src/utf32_to_utf16.c    -o ./src/utf32u_to_utf16ux.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32ux_to_utf16ux.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf16_to_utf32.c    -o ./src/utf16_to_utf32.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                      -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16_to_utf32x.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                         -DSWAP_UTF16              ./src/utf16_to_utf32.c    -o ./src/utf16x_to_utf32.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                         -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16x_to_utf32x.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED                           ./src/utf16_to_utf32.c    -o ./src/utf16_to_utf32u.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED              -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16_to_utf32ux.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED -DSWAP_UTF16              ./src/utf16_to_utf32.c    -o ./src/utf16x_to_utf32u.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16x_to_utf32ux.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                               ./src/utf16_to_utf32.c    -o ./src/utf16u_to_utf32.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                  -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16u_to_utf32x.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                     -DSWAP_UTF16              ./src/utf16_to_utf32.c    -o ./src/utf16ux_to_utf32.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                     -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16ux_to_utf32x.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED                           ./src/utf16_to_utf32.c    -o ./src/utf16u_to_utf32u.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED              -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16u_to_utf32ux.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED -DSWAP_UTF16              ./src/utf16_to_utf32.c    -o ./src/utf16ux_to_utf32u.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16ux_to_utf32ux.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf32_to_utf8.c     -o ./src/utf32_to_utf8.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                      -DSWAP_UTF32 ./src/utf32_to_utf8.c     -o ./src/utf32x_to_utf8.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                               ./src/utf32_to_utf8.c     -o ./src/utf32u_to_utf8.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                  -DSWAP_UTF32 ./src/utf32_to_utf8.c     -o ./src/utf32ux_to_utf8.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf8_to_utf32.c     -o ./src/utf8_to_utf32.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                      -DSWAP_UTF32 ./src/utf8_to_utf32.c     -o ./src/utf8_to_utf32x.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED                           ./src/utf8_to_utf32.c     -o ./src/utf8_to_utf32u.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED              -DSWAP_UTF32 ./src/utf8_to_utf32.c     -o ./src/utf8_to_utf32ux.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf16_to_utf8.c     -o ./src/utf16_to_utf8.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                         -DSWAP_UTF16              ./src/utf16_to_utf8.c     -o ./src/utf16x_to_utf8.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                               ./src/utf16_to_utf8.c     -o ./src/utf16u_to_utf8.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                     -DSWAP_UTF16              ./src/utf16_to_utf8.c     -o ./src/utf16ux_to_utf8.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf8_to_utf16.c     -o ./src/utf8_to_utf16.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                         -DSWAP_UTF16              ./src/utf8_to_utf16.c     -o ./src/utf8_to_utf16x.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED                           ./src/utf8_to_utf16.c     -o ./src/utf8_to_utf16u.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED -DSWAP_UTF16              ./src/utf8_to_utf16.c     -o ./src/utf8_to_utf16ux.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf8_to_utf16_one.c -o ./src/utf8_to_utf16_one.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf16_to_utf8_one.c -o ./src/utf16_to_utf8_one.o
gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf8_cstd.c         -o ./src/utf8_cstd.o
ar -crs libutf16.a           \
 ./src/utf32_to_utf16.o      \
 ./src/utf32x_to_utf16.o     \
 ./src/utf32_to_utf16x.o     \
 ./src/utf32x_to_utf16x.o    \
 ./src/utf32_to_utf16u.o     \
 ./src/utf32x_to_utf16u.o    \
 ./src/utf32_to_utf16ux.o    \
 ./src/utf32x_to_utf16ux.o   \
 ./src/utf32u_to_utf16.o     \
 ./src/utf32ux_to_utf16.o    \
 ./src/utf32u_to_utf16x.o    \
 ./src/utf32ux_to_utf16x.o   \
 ./src/utf32u_to_utf16u.o    \
 ./src/utf32ux_to_utf16u.o   \
 ./src/utf32u_to_utf16ux.o   \
 ./src/utf32ux_to_utf16ux.o  \
 ./src/utf16_to_utf32.o      \
 ./src/utf16_to_utf32x.o     \
 ./src/utf16x_to_utf32.o     \
 ./src/utf16x_to_utf32x.o    \
 ./src/utf16_to_utf32u.o     \
 ./src/utf16_to_utf32ux.o    \
 ./src/utf16x_to_utf32u.o    \
 ./src/utf16x_to_utf32ux.o   \
 ./src/utf16u_to_utf32.o     \
 ./src/utf16u_to_utf32x.o    \
 ./src/utf16ux_to_utf32.o    \
 ./src/utf16ux_to_utf32x.o   \
 ./src/utf16u_to_utf32u.o    \
 ./src/utf16u_to_utf32ux.o   \
 ./src/utf16ux_to_utf32u.o   \
 ./src/utf16ux_to_utf32ux.o  \
 ./src/utf32_to_utf8.o       \
 ./src/utf32x_to_utf8.o      \
 ./src/utf32u_to_utf8.o      \
 ./src/utf32ux_to_utf8.o     \
 ./src/utf8_to_utf32.o       \
 ./src/utf8_to_utf32x.o      \
 ./src/utf8_to_utf32u.o      \
 ./src/utf8_to_utf32ux.o     \
 ./src/utf16_to_utf8.o       \
 ./src/utf16x_to_utf8.o      \
 ./src/utf16u_to_utf8.o      \
 ./src/utf16ux_to_utf8.o     \
 ./src/utf8_to_utf16.o       \
 ./src/utf8_to_utf16x.o      \
 ./src/utf8_to_utf16u.o      \
 ./src/utf8_to_utf16ux.o     \
 ./src/utf8_to_utf16_one.o   \
 ./src/utf16_to_utf8_one.o   \
 ./src/utf8_cstd.o

or MSVC:
cl /O2 /I. /c /Wall                                                                   .\src\utf32_to_utf16.c    /Fo.\src\utf32_to_utf16.obj
cl /O2 /I. /c /Wall                                                      /DSWAP_UTF32 .\src\utf32_to_utf16.c    /Fo.\src\utf32x_to_utf16.obj
cl /O2 /I. /c /Wall                                         /DSWAP_UTF16              .\src\utf32_to_utf16.c    /Fo.\src\utf32_to_utf16x.obj
cl /O2 /I. /c /Wall                                         /DSWAP_UTF16 /DSWAP_UTF32 .\src\utf32_to_utf16.c    /Fo.\src\utf32x_to_utf16x.obj
cl /O2 /I. /c /Wall                     /DUTF_PUT_UNALIGNED                           .\src\utf32_to_utf16.c    /Fo.\src\utf32_to_utf16u.obj
cl /O2 /I. /c /Wall                     /DUTF_PUT_UNALIGNED              /DSWAP_UTF32 .\src\utf32_to_utf16.c    /Fo.\src\utf32x_to_utf16u.obj
cl /O2 /I. /c /Wall                     /DUTF_PUT_UNALIGNED /DSWAP_UTF16              .\src\utf32_to_utf16.c    /Fo.\src\utf32_to_utf16ux.obj
cl /O2 /I. /c /Wall                     /DUTF_PUT_UNALIGNED /DSWAP_UTF16 /DSWAP_UTF32 .\src\utf32_to_utf16.c    /Fo.\src\utf32x_to_utf16ux.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED                                               .\src\utf32_to_utf16.c    /Fo.\src\utf32u_to_utf16.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED                                  /DSWAP_UTF32 .\src\utf32_to_utf16.c    /Fo.\src\utf32ux_to_utf16.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED                     /DSWAP_UTF16              .\src\utf32_to_utf16.c    /Fo.\src\utf32u_to_utf16x.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED                     /DSWAP_UTF16 /DSWAP_UTF32 .\src\utf32_to_utf16.c    /Fo.\src\utf32ux_to_utf16x.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED /DUTF_PUT_UNALIGNED                           .\src\utf32_to_utf16.c    /Fo.\src\utf32u_to_utf16u.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED /DUTF_PUT_UNALIGNED              /DSWAP_UTF32 .\src\utf32_to_utf16.c    /Fo.\src\utf32ux_to_utf16u.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED /DUTF_PUT_UNALIGNED /DSWAP_UTF16              .\src\utf32_to_utf16.c    /Fo.\src\utf32u_to_utf16ux.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED /DUTF_PUT_UNALIGNED /DSWAP_UTF16 /DSWAP_UTF32 .\src\utf32_to_utf16.c    /Fo.\src\utf32ux_to_utf16ux.obj
cl /O2 /I. /c /Wall                                                                   .\src\utf16_to_utf32.c    /Fo.\src\utf16_to_utf32.obj
cl /O2 /I. /c /Wall                                                      /DSWAP_UTF32 .\src\utf16_to_utf32.c    /Fo.\src\utf16_to_utf32x.obj
cl /O2 /I. /c /Wall                                         /DSWAP_UTF16              .\src\utf16_to_utf32.c    /Fo.\src\utf16x_to_utf32.obj
cl /O2 /I. /c /Wall                                         /DSWAP_UTF16 /DSWAP_UTF32 .\src\utf16_to_utf32.c    /Fo.\src\utf16x_to_utf32x.obj
cl /O2 /I. /c /Wall                     /DUTF_PUT_UNALIGNED                           .\src\utf16_to_utf32.c    /Fo.\src\utf16_to_utf32u.obj
cl /O2 /I. /c /Wall                     /DUTF_PUT_UNALIGNED              /DSWAP_UTF32 .\src\utf16_to_utf32.c    /Fo.\src\utf16_to_utf32ux.obj
cl /O2 /I. /c /Wall                     /DUTF_PUT_UNALIGNED /DSWAP_UTF16              .\src\utf16_to_utf32.c    /Fo.\src\utf16x_to_utf32u.obj
cl /O2 /I. /c /Wall                     /DUTF_PUT_UNALIGNED /DSWAP_UTF16 /DSWAP_UTF32 .\src\utf16_to_utf32.c    /Fo.\src\utf16x_to_utf32ux.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED                                               .\src\utf16_to_utf32.c    /Fo.\src\utf16u_to_utf32.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED                                  /DSWAP_UTF32 .\src\utf16_to_utf32.c    /Fo.\src\utf16u_to_utf32x.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED                     /DSWAP_UTF16              .\src\utf16_to_utf32.c    /Fo.\src\utf16ux_to_utf32.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED                     /DSWAP_UTF16 /DSWAP_UTF32 .\src\utf16_to_utf32.c    /Fo.\src\utf16ux_to_utf32x.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED /DUTF_PUT_UNALIGNED                           .\src\utf16_to_utf32.c    /Fo.\src\utf16u_to_utf32u.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED /DUTF_PUT_UNALIGNED              /DSWAP_UTF32 .\src\utf16_to_utf32.c    /Fo.\src\utf16u_to_utf32ux.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED /DUTF_PUT_UNALIGNED /DSWAP_UTF16              .\src\utf16_to_utf32.c    /Fo.\src\utf16ux_to_utf32u.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED /DUTF_PUT_UNALIGNED /DSWAP_UTF16 /DSWAP_UTF32 .\src\utf16_to_utf32.c    /Fo.\src\utf16ux_to_utf32ux.obj
cl /O2 /I. /c /Wall                                                                   .\src\utf32_to_utf8.c     /Fo.\src\utf32_to_utf8.obj
cl /O2 /I. /c /Wall                                                      /DSWAP_UTF32 .\src\utf32_to_utf8.c     /Fo.\src\utf32x_to_utf8.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED                                               .\src\utf32_to_utf8.c     /Fo.\src\utf32u_to_utf8.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED                                  /DSWAP_UTF32 .\src\utf32_to_utf8.c     /Fo.\src\utf32ux_to_utf8.obj
cl /O2 /I. /c /Wall                                                                   .\src\utf8_to_utf32.c     /Fo.\src\utf8_to_utf32.obj
cl /O2 /I. /c /Wall                                                      /DSWAP_UTF32 .\src\utf8_to_utf32.c     /Fo.\src\utf8_to_utf32x.obj
cl /O2 /I. /c /Wall                     /DUTF_PUT_UNALIGNED                           .\src\utf8_to_utf32.c     /Fo.\src\utf8_to_utf32u.obj
cl /O2 /I. /c /Wall                     /DUTF_PUT_UNALIGNED              /DSWAP_UTF32 .\src\utf8_to_utf32.c     /Fo.\src\utf8_to_utf32ux.obj
cl /O2 /I. /c /Wall                                                                   .\src\utf16_to_utf8.c     /Fo.\src\utf16_to_utf8.obj
cl /O2 /I. /c /Wall                                         /DSWAP_UTF16              .\src\utf16_to_utf8.c     /Fo.\src\utf16x_to_utf8.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED                                               .\src\utf16_to_utf8.c     /Fo.\src\utf16u_to_utf8.obj
cl /O2 /I. /c /Wall /DUTF_GET_UNALIGNED                     /DSWAP_UTF16              .\src\utf16_to_utf8.c     /Fo.\src\utf16ux_to_utf8.obj
cl /O2 /I. /c /Wall                                                                   .\src\utf8_to_utf16.c     /Fo.\src\utf8_to_utf16.obj
cl /O2 /I. /c /Wall                                         /DSWAP_UTF16              .\src\utf8_to_utf16.c     /Fo.\src\utf8_to_utf16x.obj
cl /O2 /I. /c /Wall                     /DUTF_PUT_UNALIGNED                           .\src\utf8_to_utf16.c     /Fo.\src\utf8_to_utf16u.obj
cl /O2 /I. /c /Wall                     /DUTF_PUT_UNALIGNED /DSWAP_UTF16              .\src\utf8_to_utf16.c     /Fo.\src\utf8_to_utf16ux.obj
cl /O2 /I. /c /Wall                                                                   .\src\utf8_to_utf16_one.c /Fo.\src\utf8_to_utf16_one.obj
cl /O2 /I. /c /Wall                                                                   .\src\utf16_to_utf8_one.c /Fo.\src\utf16_to_utf8_one.obj
cl /O2 /I. /c /Wall                                                                   .\src\utf8_cstd.c         /Fo.\src\utf8_cstd.obj
lib /out:utf16.a               ^
 .\src\utf32_to_utf16.obj      ^
 .\src\utf32x_to_utf16.obj     ^
 .\src\utf32_to_utf16x.obj     ^
 .\src\utf32x_to_utf16x.obj    ^
 .\src\utf32_to_utf16u.obj     ^
 .\src\utf32x_to_utf16u.obj    ^
 .\src\utf32_to_utf16ux.obj    ^
 .\src\utf32x_to_utf16ux.obj   ^
 .\src\utf32u_to_utf16.obj     ^
 .\src\utf32ux_to_utf16.obj    ^
 .\src\utf32u_to_utf16x.obj    ^
 .\src\utf32ux_to_utf16x.obj   ^
 .\src\utf32u_to_utf16u.obj    ^
 .\src\utf32ux_to_utf16u.obj   ^
 .\src\utf32u_to_utf16ux.obj   ^
 .\src\utf32ux_to_utf16ux.obj  ^
 .\src\utf16_to_utf32.obj      ^
 .\src\utf16_to_utf32x.obj     ^
 .\src\utf16x_to_utf32.obj     ^
 .\src\utf16x_to_utf32x.obj    ^
 .\src\utf16_to_utf32u.obj     ^
 .\src\utf16_to_utf32ux.obj    ^
 .\src\utf16x_to_utf32u.obj    ^
 .\src\utf16x_to_utf32ux.obj   ^
 .\src\utf16u_to_utf32.obj     ^
 .\src\utf16u_to_utf32x.obj    ^
 .\src\utf16ux_to_utf32.obj    ^
 .\src\utf16ux_to_utf32x.obj   ^
 .\src\utf16u_to_utf32u.obj    ^
 .\src\utf16u_to_utf32ux.obj   ^
 .\src\utf16ux_to_utf32u.obj   ^
 .\src\utf16ux_to_utf32ux.obj  ^
 .\src\utf32_to_utf8.obj       ^
 .\src\utf32x_to_utf8.obj      ^
 .\src\utf32u_to_utf8.obj      ^
 .\src\utf32ux_to_utf8.obj     ^
 .\src\utf8_to_utf32.obj       ^
 .\src\utf8_to_utf32x.obj      ^
 .\src\utf8_to_utf32u.obj      ^
 .\src\utf8_to_utf32ux.obj     ^
 .\src\utf16_to_utf8.obj       ^
 .\src\utf16x_to_utf8.obj      ^
 .\src\utf16u_to_utf8.obj      ^
 .\src\utf16ux_to_utf8.obj     ^
 .\src\utf8_to_utf16.obj       ^
 .\src\utf8_to_utf16x.obj      ^
 .\src\utf8_to_utf16u.obj      ^
 .\src\utf8_to_utf16ux.obj     ^
 .\src\utf8_to_utf16_one.obj   ^
 .\src\utf16_to_utf8_one.obj   ^
 .\src\utf8_cstd.obj
