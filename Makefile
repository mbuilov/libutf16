all: libutf16.a

UTF32_TO_UTF16 = src/utf32_to_utf16.c libutf16/utf32_to_utf16.h \
  libutf16/utf16_char.h libutf16/utf16_swap.h src/utf16_internal.h

UTF16_TO_UTF32 = src/utf16_to_utf32.c libutf16/utf16_to_utf32.h \
  libutf16/utf16_char.h libutf16/utf16_swap.h src/utf16_internal.h

UTF32_TO_UTF8 = src/utf32_to_utf8.c libutf16/utf32_to_utf8.h \
  libutf16/utf16_char.h libutf16/utf16_swap.h src/utf16_internal.h

UTF8_TO_UTF32 = src/utf8_to_utf32.c libutf16/utf8_to_utf32.h \
  libutf16/utf16_char.h libutf16/utf16_swap.h src/utf16_internal.h

UTF16_TO_UTF8 = src/utf16_to_utf8.c libutf16/utf16_to_utf8.h \
  libutf16/utf16_char.h libutf16/utf16_swap.h src/utf16_internal.h

UTF8_TO_UTF16 = src/utf8_to_utf16.c libutf16/utf8_to_utf16.h \
  libutf16/utf16_char.h libutf16/utf16_swap.h src/utf16_internal.h

UTF8_TO_UTF16_ONE = src/utf8_to_utf16_one.c libutf16/utf8_to_utf16_one.h \
  libutf16/utf16_char.h

UTF16_TO_UTF8_ONE = src/utf16_to_utf8_one.c libutf16/utf16_to_utf8_one.h \
  libutf16/utf16_char.h

UTF8_CSTD = src/utf8_cstd.c libutf16/utf8_cstd.h libutf16/utf16_char.h \
  libutf16/utf8_to_utf16.h libutf16/utf16_to_utf8.h \
  libutf16/utf8_to_utf32.h libutf16/utf32_to_utf8.h \
  libutf16/utf16_to_utf32.h libutf16/utf32_to_utf16.h \
  libutf16/utf8_to_utf16_one.h libutf16/utf16_to_utf8_one.h

./src/utf32_to_utf16.o:     $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf32_to_utf16.c    -o ./src/utf32_to_utf16.o
./src/utf32x_to_utf16.o:    $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                      -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32x_to_utf16.o
./src/utf32_to_utf16x.o:    $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                         -DSWAP_UTF16              ./src/utf32_to_utf16.c    -o ./src/utf32_to_utf16x.o
./src/utf32x_to_utf16x.o:   $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                         -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32x_to_utf16x.o
./src/utf32_to_utf16u.o:    $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED                           ./src/utf32_to_utf16.c    -o ./src/utf32_to_utf16u.o
./src/utf32x_to_utf16u.o:   $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED              -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32x_to_utf16u.o
./src/utf32_to_utf16ux.o:   $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED -DSWAP_UTF16              ./src/utf32_to_utf16.c    -o ./src/utf32_to_utf16ux.o
./src/utf32x_to_utf16ux.o:  $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32x_to_utf16ux.o
./src/utf32u_to_utf16.o:    $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                               ./src/utf32_to_utf16.c    -o ./src/utf32u_to_utf16.o
./src/utf32ux_to_utf16.o:   $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                  -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32ux_to_utf16.o
./src/utf32u_to_utf16x.o:   $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                     -DSWAP_UTF16              ./src/utf32_to_utf16.c    -o ./src/utf32u_to_utf16x.o
./src/utf32ux_to_utf16x.o:  $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                     -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32ux_to_utf16x.o
./src/utf32u_to_utf16u.o:   $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED                           ./src/utf32_to_utf16.c    -o ./src/utf32u_to_utf16u.o
./src/utf32ux_to_utf16u.o:  $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED              -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32ux_to_utf16u.o
./src/utf32u_to_utf16ux.o:  $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED -DSWAP_UTF16              ./src/utf32_to_utf16.c    -o ./src/utf32u_to_utf16ux.o
./src/utf32ux_to_utf16ux.o: $(UTF32_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf32_to_utf16.c    -o ./src/utf32ux_to_utf16ux.o
./src/utf16_to_utf32.o:     $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf16_to_utf32.c    -o ./src/utf16_to_utf32.o
./src/utf16_to_utf32x.o:    $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                      -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16_to_utf32x.o
./src/utf16x_to_utf32.o:    $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                         -DSWAP_UTF16              ./src/utf16_to_utf32.c    -o ./src/utf16x_to_utf32.o
./src/utf16x_to_utf32x.o:   $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                         -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16x_to_utf32x.o
./src/utf16_to_utf32u.o:    $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED                           ./src/utf16_to_utf32.c    -o ./src/utf16_to_utf32u.o
./src/utf16_to_utf32ux.o:   $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED              -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16_to_utf32ux.o
./src/utf16x_to_utf32u.o:   $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED -DSWAP_UTF16              ./src/utf16_to_utf32.c    -o ./src/utf16x_to_utf32u.o
./src/utf16x_to_utf32ux.o:  $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16x_to_utf32ux.o
./src/utf16u_to_utf32.o:    $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                               ./src/utf16_to_utf32.c    -o ./src/utf16u_to_utf32.o
./src/utf16u_to_utf32x.o:   $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                  -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16u_to_utf32x.o
./src/utf16ux_to_utf32.o:   $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                     -DSWAP_UTF16              ./src/utf16_to_utf32.c    -o ./src/utf16ux_to_utf32.o
./src/utf16ux_to_utf32x.o:  $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                     -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16ux_to_utf32x.o
./src/utf16u_to_utf32u.o:   $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED                           ./src/utf16_to_utf32.c    -o ./src/utf16u_to_utf32u.o
./src/utf16u_to_utf32ux.o:  $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED              -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16u_to_utf32ux.o
./src/utf16ux_to_utf32u.o:  $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED -DSWAP_UTF16              ./src/utf16_to_utf32.c    -o ./src/utf16ux_to_utf32u.o
./src/utf16ux_to_utf32ux.o: $(UTF16_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED -DUTF_PUT_UNALIGNED -DSWAP_UTF16 -DSWAP_UTF32 ./src/utf16_to_utf32.c    -o ./src/utf16ux_to_utf32ux.o
./src/utf32_to_utf8.o:      $(UTF32_TO_UTF8)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf32_to_utf8.c     -o ./src/utf32_to_utf8.o
./src/utf32x_to_utf8.o:     $(UTF32_TO_UTF8)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                      -DSWAP_UTF32 ./src/utf32_to_utf8.c     -o ./src/utf32x_to_utf8.o
./src/utf32u_to_utf8.o:     $(UTF32_TO_UTF8)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                               ./src/utf32_to_utf8.c     -o ./src/utf32u_to_utf8.o
./src/utf32ux_to_utf8.o:    $(UTF32_TO_UTF8)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                  -DSWAP_UTF32 ./src/utf32_to_utf8.c     -o ./src/utf32ux_to_utf8.o
./src/utf8_to_utf32.o:      $(UTF8_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf8_to_utf32.c     -o ./src/utf8_to_utf32.o
./src/utf8_to_utf32x.o:     $(UTF8_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                      -DSWAP_UTF32 ./src/utf8_to_utf32.c     -o ./src/utf8_to_utf32x.o
./src/utf8_to_utf32u.o:     $(UTF8_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED                           ./src/utf8_to_utf32.c     -o ./src/utf8_to_utf32u.o
./src/utf8_to_utf32ux.o:    $(UTF8_TO_UTF32)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED              -DSWAP_UTF32 ./src/utf8_to_utf32.c     -o ./src/utf8_to_utf32ux.o
./src/utf16_to_utf8.o:      $(UTF16_TO_UTF8)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf16_to_utf8.c     -o ./src/utf16_to_utf8.o
./src/utf16x_to_utf8.o:     $(UTF16_TO_UTF8)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                         -DSWAP_UTF16              ./src/utf16_to_utf8.c     -o ./src/utf16x_to_utf8.o
./src/utf16u_to_utf8.o:     $(UTF16_TO_UTF8)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                                               ./src/utf16_to_utf8.c     -o ./src/utf16u_to_utf8.o
./src/utf16ux_to_utf8.o:    $(UTF16_TO_UTF8)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra -DUTF_GET_UNALIGNED                     -DSWAP_UTF16              ./src/utf16_to_utf8.c     -o ./src/utf16ux_to_utf8.o
./src/utf8_to_utf16.o:      $(UTF8_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf8_to_utf16.c     -o ./src/utf8_to_utf16.o
./src/utf8_to_utf16x.o:     $(UTF8_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                         -DSWAP_UTF16              ./src/utf8_to_utf16.c     -o ./src/utf8_to_utf16x.o
./src/utf8_to_utf16u.o:     $(UTF8_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED                           ./src/utf8_to_utf16.c     -o ./src/utf8_to_utf16u.o
./src/utf8_to_utf16ux.o:    $(UTF8_TO_UTF16)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                     -DUTF_PUT_UNALIGNED -DSWAP_UTF16              ./src/utf8_to_utf16.c     -o ./src/utf8_to_utf16ux.o
./src/utf8_to_utf16_one.o:  $(UTF8_TO_UTF16_ONE)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf8_to_utf16_one.c -o ./src/utf8_to_utf16_one.o
./src/utf16_to_utf8_one.o:  $(UTF16_TO_UTF8_ONE)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf16_to_utf8_one.c -o ./src/utf16_to_utf8_one.o
./src/utf8_cstd.o:          $(UTF8_CSTD)
	gcc -g -O2 -I. -c -Wall -pedantic -Wextra                                                                   ./src/utf8_cstd.c         -o ./src/utf8_cstd.o

OBJS = \
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

libutf16.a: $(OBJS)
	ar -crs libutf16.a $(OBJS)

test: tests/test.c libutf16.a
	gcc -g -O2 -I. -DCHECK_UTF8_LOCALE -DLIBC_GLIBC -Wall -pedantic -Wextra tests/test.c -o test libutf16.a

check: test
	./test

clean:
	rm -f src/*.o libutf16.a test
