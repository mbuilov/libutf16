/* test.c */

/* compile as:
  gcc -Wall -pedantic -Wextra tests/test.c -I. libutf16.a -o test
   or
  cl /Wall /wd4820 /wd5045 /wd4711 /wd4710 /Fetest.exe tests\test.c /I. /link utf16.a
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <errno.h>
#ifdef CHECK_UTF8_LOCALE
#include <locale.h>
#include <wchar.h>
#include <uchar.h>
#endif

#include "libutf16/utf16_to_utf8.h"
#include "libutf16/utf32_to_utf8.h"
#include "libutf16/utf8_to_utf16.h"
#include "libutf16/utf8_to_utf32.h"
#include "libutf16/utf16_to_utf32.h"
#include "libutf16/utf32_to_utf16.h"
#include "libutf16/utf8_to_utf16_one.h"
#include "libutf16/utf16_to_utf8_one.h"
#include "libutf16/utf8_cstd.h"

static unsigned long long test_number = 0;

#ifdef CHECK_UTF8_LOCALE
#define LIBC_TEST(expr) do { \
	if (!(expr)) { \
		printf("libc test failed: after test %llu (at line = %d)\n", test_number, __LINE__); \
	} \
} while (0)
#endif

#define TEST(expr) do { \
	if (!(expr)) { \
		printf("test %llu failed (at line = %d)\n", test_number, __LINE__); \
		return 1; \
	} \
	if (!(test_number % 10000000)) \
		printf("test %llu ok\n", test_number); \
	test_number++; \
} while (0)

#define NULLSZ(f) \
	{ \
		const size_t sz = f(NULL, 0); \
		TEST(!sz); \
	}

#define FUNC(f,a)  f a

#define TESTSZ(sz,f,t,s,e,dst_sz) \
	{ \
		const t *w = s; \
		const size_t sz = FUNC(f, (&w sz((size_t)(e - w)))); \
		TEST(sz == dst_sz); \
		TEST(w == s); \
	}

#define TESTCONV(sz,f,t,s,e,z,xt,dt,et,buf,buf_sz,dst_sz) \
	{ \
		unsigned i = 0, next_i = 0; \
		unsigned j = 0, next_j = 0; \
		for (; i <= buf_sz; i = next_i, j = next_j) { \
			unsigned l = i; \
			unsigned step = initial_step; \
			/* zero input buffer can't be passed to _z_-functions */ \
			if (z && j == (size_t)(e - s)) \
				break; \
			for (next_j = 0; l <= buf_sz; l += ((step <= buf_sz - l) ? step++ : (step = 1))) { \
				unsigned q = l; \
				for (; q < buf_sz; q++) { \
					const xt x = (xt)(q + 10); \
					memcpy(&buf[q], &x, sizeof(x)); \
				} \
				{ \
					dt *b = (dt*)memset(buf + i, 0, sizeof(*buf)*(((l < dst_sz) ? l : dst_sz) - i)); \
					const t *w = s + j; \
					const size_t sz = FUNC(f, (&w, &b, l - i sz((size_t)(e - w)))); \
					TEST(sz == dst_sz - i); \
					TEST(b <= &buf[l]); \
					TEST(w <= e); \
					TEST(l < dst_sz || b == &buf[dst_sz]); \
					TEST(l < dst_sz || w == e); \
					TEST(!memcmp(buf + i, et + i, sizeof(*buf)*((size_t)(b - buf) - i))); \
					for (q = l < dst_sz ? l : dst_sz;  q < buf_sz; q++) { \
						const xt x = (xt)(q + 10); \
						TEST(!memcmp(&buf[q], &x, sizeof(x))); \
					} \
					TEST((b != buf + i) == (w != s + j)); \
					if (b != buf + i && !next_j) { \
						next_i = (unsigned)(b - buf); \
						next_j = (unsigned)(w - s); \
					} \
				} \
				{ \
					dt *b = (dt*)memset(buf + i, 0, sizeof(*buf)*(((l < dst_sz) ? l : dst_sz) - i)); \
					const t *w = s + j; \
					const size_t sz = FUNC(f##_partial, (&w, &b, l - i sz((size_t)(e - w)))); \
					if (l >= dst_sz) \
						TEST(sz == dst_sz - i); \
					else \
						TEST(sz > l - i); \
					TEST(b <= &buf[l]); \
					TEST(w <= e); \
					TEST(l < dst_sz || b == &buf[dst_sz]); \
					TEST(l < dst_sz || w == e); \
					TEST(!memcmp(buf + i, et + i, sizeof(*buf)*((size_t)(b - buf) - i))); \
					for (q = l < dst_sz ? l : dst_sz;  q < buf_sz; q++) { \
						const xt x = (xt)(q + 10); \
						TEST(!memcmp(&buf[q], &x, sizeof(x))); \
					} \
					TEST((b != buf + i) == (w != s + j)); \
				} \
				/* zero output buffer can't be passed to _unsafe()-functions */ \
				if (i < dst_sz) { \
					dt *const b = (dt*)memset(buf + i, 0, sizeof(*buf)*(dst_sz - i)); \
					const t *const w = s + j; \
					FUNC(f##_unsafe, (w, b sz((size_t)(e - w)))); \
					TEST(!memcmp(buf + i, et + i, sizeof(*buf)*(dst_sz - i))); \
					for (q = dst_sz;  q < buf_sz; q++) { \
						const xt x = (xt)(q + 10); \
						TEST(!memcmp(&buf[q], &x, sizeof(x))); \
					} \
				} \
			} \
			TEST(next_j || i == dst_sz); \
			if (!next_j) \
				break; \
		} \
	}

#define SZ(x) ,x
#define EMPTY(x)

static int z(void)  {return 0;}
static int nz(void) {return 1;}

static int test_utf16_to_utf8(
	const unsigned initial_step,
	const unsigned char *const utf16_le_be[2],
	const unsigned utf16_sz,
	const unsigned utf8_sz,
	const utf8_char_t utf8[/*utf8_sz*/],
	const unsigned utf8_buf_sz/*>=utf8_sz*/,
	utf8_char_t utf8_buf[/*utf8_buf_sz*/])
{
	const unsigned le = 1;
	const utf16_char_t *const src16 = (const utf16_char_t*)utf16_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf16_char_t *const src16_e = src16 + utf16_sz - 1/*BOM*/;
	const utf16_char_unaligned_t *const src16u = (const utf16_char_unaligned_t*)utf16_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf16_char_unaligned_t *const src16u_e = src16u + utf16_sz - 1/*BOM*/;
	const utf16_char_t *const src16x = (const utf16_char_t*)utf16_le_be[!!*(const char*)&le] + 1/*BOM*/;
	const utf16_char_t *const src16x_e = src16x + utf16_sz - 1/*BOM*/;
	const utf16_char_unaligned_t *const src16ux = (const utf16_char_unaligned_t*)utf16_le_be[!!*(const char*)&le] + 1/*BOM*/;
	const utf16_char_unaligned_t *const src16ux_e = src16ux + utf16_sz - 1/*BOM*/;
	NULLSZ(utf16_to_utf8_size)
	NULLSZ(utf16u_to_utf8_size)
	NULLSZ(utf16x_to_utf8_size)
	NULLSZ(utf16ux_to_utf8_size)
	TEST(utf16_is_bom_be(utf16_le_be[1][0], utf16_le_be[1][1]));
	TEST(utf16_is_bom_le(utf16_le_be[0][0], utf16_le_be[0][1]));
	TESTSZ(SZ, utf16_to_utf8_size, utf16_char_t, src16, src16_e, utf8_sz)
	TESTSZ(SZ, utf16u_to_utf8_size, utf16_char_unaligned_t, src16u, src16u_e, utf8_sz)
	TESTSZ(SZ, utf16x_to_utf8_size, utf16_char_t, src16x, src16x_e, utf8_sz)
	TESTSZ(SZ, utf16ux_to_utf8_size, utf16_char_unaligned_t, src16ux, src16ux_e, utf8_sz)
	TESTSZ(EMPTY, utf16_to_utf8_z_size, utf16_char_t, src16, src16_e, utf8_sz)
	TESTSZ(EMPTY, utf16u_to_utf8_z_size, utf16_char_unaligned_t, src16u, src16u_e, utf8_sz)
	TESTSZ(EMPTY, utf16x_to_utf8_z_size, utf16_char_t, src16x, src16x_e, utf8_sz)
	TESTSZ(EMPTY, utf16ux_to_utf8_z_size, utf16_char_unaligned_t, src16ux, src16ux_e, utf8_sz)
	TESTCONV(SZ, utf16_to_utf8, utf16_char_t, src16, src16_e, z(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(SZ, utf16u_to_utf8, utf16_char_unaligned_t, src16u, src16u_e, z(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(SZ, utf16x_to_utf8, utf16_char_t, src16x, src16x_e, z(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(SZ, utf16ux_to_utf8, utf16_char_unaligned_t, src16ux, src16ux_e, z(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf16_to_utf8_z, utf16_char_t, src16, src16_e, nz(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf16u_to_utf8_z, utf16_char_unaligned_t, src16u, src16u_e, nz(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf16x_to_utf8_z, utf16_char_t, src16x, src16x_e, nz(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf16ux_to_utf8_z, utf16_char_unaligned_t, src16ux, src16ux_e, nz(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	return 0;
}

static int test_utf32_to_utf8(
	const unsigned initial_step,
	const unsigned char *const utf32_le_be[2],
	const unsigned utf32_sz,
	const unsigned utf8_sz,
	const utf8_char_t utf8[/*utf8_sz*/],
	const unsigned utf8_buf_sz/*>=utf8_sz*/,
	utf8_char_t utf8_buf[/*utf8_buf_sz*/])
{
	const unsigned le = 1;
	const utf32_char_t *const src32 = (const utf32_char_t*)utf32_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf32_char_t *const src32_e = src32 + utf32_sz - 1/*BOM*/;
	const utf32_char_unaligned_t *const src32u = (const utf32_char_unaligned_t*)utf32_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf32_char_unaligned_t *const src32u_e = src32u + utf32_sz - 1/*BOM*/;
	const utf32_char_t *const src32x = (const utf32_char_t*)utf32_le_be[!!*(const char*)&le] + 1/*BOM*/;
	const utf32_char_t *const src32x_e = src32x + utf32_sz - 1/*BOM*/;
	const utf32_char_unaligned_t *const src32ux = (const utf32_char_unaligned_t*)utf32_le_be[!!*(const char*)&le] + 1/*BOM*/;
	const utf32_char_unaligned_t *const src32ux_e = src32ux + utf32_sz - 1/*BOM*/;
	NULLSZ(utf32_to_utf8_size)
	NULLSZ(utf32u_to_utf8_size)
	NULLSZ(utf32x_to_utf8_size)
	NULLSZ(utf32ux_to_utf8_size)
	TEST(utf32_is_bom_be(utf32_le_be[1][0], utf32_le_be[1][1], utf32_le_be[1][2], utf32_le_be[1][3]));
	TEST(utf32_is_bom_le(utf32_le_be[0][0], utf32_le_be[0][1], utf32_le_be[0][2], utf32_le_be[0][3]));
	TESTSZ(SZ, utf32_to_utf8_size, utf32_char_t, src32, src32_e, utf8_sz)
	TESTSZ(SZ, utf32u_to_utf8_size, utf32_char_unaligned_t, src32u, src32u_e, utf8_sz)
	TESTSZ(SZ, utf32x_to_utf8_size, utf32_char_t, src32x, src32x_e, utf8_sz)
	TESTSZ(SZ, utf32ux_to_utf8_size, utf32_char_unaligned_t, src32ux, src32ux_e, utf8_sz)
	TESTSZ(EMPTY, utf32_to_utf8_z_size, utf32_char_t, src32, src32_e, utf8_sz)
	TESTSZ(EMPTY, utf32u_to_utf8_z_size, utf32_char_unaligned_t, src32u, src32u_e, utf8_sz)
	TESTSZ(EMPTY, utf32x_to_utf8_z_size, utf32_char_t, src32x, src32x_e, utf8_sz)
	TESTSZ(EMPTY, utf32ux_to_utf8_z_size, utf32_char_unaligned_t, src32ux, src32ux_e, utf8_sz)
	TESTCONV(SZ, utf32_to_utf8, utf32_char_t, src32, src32_e, z(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(SZ, utf32u_to_utf8, utf32_char_unaligned_t, src32u, src32u_e, z(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(SZ, utf32x_to_utf8, utf32_char_t, src32x, src32x_e, z(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(SZ, utf32ux_to_utf8, utf32_char_unaligned_t, src32ux, src32ux_e, z(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf32_to_utf8_z, utf32_char_t, src32, src32_e, nz(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf32u_to_utf8_z, utf32_char_unaligned_t, src32u, src32u_e, nz(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf32x_to_utf8_z, utf32_char_t, src32x, src32x_e, nz(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf32ux_to_utf8_z, utf32_char_unaligned_t, src32ux, src32ux_e, nz(), utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	return 0;
}

static int test_utf8_to_utf16(
	const unsigned initial_step,
	const unsigned char *const utf16_le_be[2],
	const unsigned utf16_sz,
	const unsigned utf8_sz,
	const utf8_char_t utf8[/*utf8_sz*/],
	const unsigned utf16_buf_sz/*>=utf16_sz*/,
	utf16_char_t utf16_buf[/*utf16_buf_sz*/])
{
	const unsigned le = 1;
	const utf16_char_t *const src16 = (const utf16_char_t*)utf16_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf16_char_t *const src16x = (const utf16_char_t*)utf16_le_be[!!*(const char*)&le] + 1/*BOM*/;
	const utf8_char_t *const utf8_e = &utf8[utf8_sz];
	NULLSZ(utf8_to_utf16_size)
	NULLSZ(utf8_to_utf16u_size)
	NULLSZ(utf8_to_utf16x_size)
	NULLSZ(utf8_to_utf16ux_size)
	TEST(utf16_is_bom_be(utf16_le_be[1][0], utf16_le_be[1][1]));
	TEST(utf16_is_bom_le(utf16_le_be[0][0], utf16_le_be[0][1]));
	TESTSZ(SZ, utf8_to_utf16_size, utf8_char_t, utf8, utf8_e, utf16_sz - 1/*BOM*/)
	TESTSZ(SZ, utf8_to_utf16u_size, utf8_char_t, utf8, utf8_e, utf16_sz - 1/*BOM*/)
	TESTSZ(SZ, utf8_to_utf16x_size, utf8_char_t, utf8, utf8_e, utf16_sz - 1/*BOM*/)
	TESTSZ(SZ, utf8_to_utf16ux_size, utf8_char_t, utf8, utf8_e, utf16_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf8_to_utf16_z_size, utf8_char_t, utf8, utf8_e, utf16_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf8_to_utf16u_z_size, utf8_char_t, utf8, utf8_e, utf16_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf8_to_utf16x_z_size, utf8_char_t, utf8, utf8_e, utf16_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf8_to_utf16ux_z_size, utf8_char_t, utf8, utf8_e, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf16, utf8_char_t, utf8, utf8_e, z(), utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf16u, utf8_char_t, utf8, utf8_e, z(), utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf16x, utf8_char_t, utf8, utf8_e, z(), utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf16ux, utf8_char_t, utf8, utf8_e, z(), utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf16_z, utf8_char_t, utf8, utf8_e, nz(), utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf16u_z, utf8_char_t, utf8, utf8_e, nz(), utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf16x_z, utf8_char_t, utf8, utf8_e, nz(), utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf16ux_z, utf8_char_t, utf8, utf8_e, nz(), utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	return 0;
}

static int test_utf8_to_utf32(
	const unsigned initial_step,
	const unsigned char *const utf32_le_be[2],
	const unsigned utf32_sz,
	const unsigned utf8_sz,
	const utf8_char_t utf8[/*utf8_sz*/],
	const unsigned utf32_buf_sz/*>=utf32_sz*/,
	utf32_char_t utf32_buf[/*utf32_buf_sz*/])
{
	const unsigned le = 1;
	const utf32_char_t *const src32 = (const utf32_char_t*)utf32_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf32_char_t *const src32x = (const utf32_char_t*)utf32_le_be[!!*(const char*)&le] + 1/*BOM*/;
	const utf8_char_t *const utf8_e = &utf8[utf8_sz];
	NULLSZ(utf8_to_utf32_size)
	NULLSZ(utf8_to_utf32u_size)
	NULLSZ(utf8_to_utf32x_size)
	NULLSZ(utf8_to_utf32ux_size)
	TEST(utf32_is_bom_be(utf32_le_be[1][0], utf32_le_be[1][1], utf32_le_be[1][2], utf32_le_be[1][3]));
	TEST(utf32_is_bom_le(utf32_le_be[0][0], utf32_le_be[0][1], utf32_le_be[0][2], utf32_le_be[0][3]));
	TESTSZ(SZ, utf8_to_utf32_size, utf8_char_t, utf8, utf8_e, utf32_sz - 1/*BOM*/)
	TESTSZ(SZ, utf8_to_utf32u_size, utf8_char_t, utf8, utf8_e, utf32_sz - 1/*BOM*/)
	TESTSZ(SZ, utf8_to_utf32x_size, utf8_char_t, utf8, utf8_e, utf32_sz - 1/*BOM*/)
	TESTSZ(SZ, utf8_to_utf32ux_size, utf8_char_t, utf8, utf8_e, utf32_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf8_to_utf32_z_size, utf8_char_t, utf8, utf8_e, utf32_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf8_to_utf32u_z_size, utf8_char_t, utf8, utf8_e, utf32_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf8_to_utf32x_z_size, utf8_char_t, utf8, utf8_e, utf32_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf8_to_utf32ux_z_size, utf8_char_t, utf8, utf8_e, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf32, utf8_char_t, utf8, utf8_e, z(), utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf32u, utf8_char_t, utf8, utf8_e, z(), utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf32x, utf8_char_t, utf8, utf8_e, z(), utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf32ux, utf8_char_t, utf8, utf8_e, z(), utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf32_z, utf8_char_t, utf8, utf8_e, nz(), utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf32u_z, utf8_char_t, utf8, utf8_e, nz(), utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf32x_z, utf8_char_t, utf8, utf8_e, nz(), utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf32ux_z, utf8_char_t, utf8, utf8_e, nz(), utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	return 0;
}

static int test_utf16_to_utf32(
	const unsigned initial_step,
	const unsigned char *const utf16_le_be[2],
	const unsigned char *const utf32_le_be[2],
	const unsigned utf16_sz,
	const unsigned utf32_sz,
	const unsigned utf32_buf_sz/*>=utf32_sz*/,
	utf32_char_t utf32_buf[/*utf32_buf_sz*/])
{
	const unsigned le = 1;
	const utf16_char_t *const src16 = (const utf16_char_t*)utf16_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf16_char_t *const src16_e = src16 + utf16_sz - 1/*BOM*/;
	const utf16_char_unaligned_t *const src16u = (const utf16_char_unaligned_t*)utf16_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf16_char_unaligned_t *const src16u_e = src16u + utf16_sz - 1/*BOM*/;
	const utf16_char_t *const src16x = (const utf16_char_t*)utf16_le_be[!!*(const char*)&le] + 1/*BOM*/;
	const utf16_char_t *const src16x_e = src16x + utf16_sz - 1/*BOM*/;
	const utf16_char_unaligned_t *const src16ux = (const utf16_char_unaligned_t*)utf16_le_be[!!*(const char*)&le] + 1/*BOM*/;
	const utf16_char_unaligned_t *const src16ux_e = src16ux + utf16_sz - 1/*BOM*/;
	const utf32_char_t *const src32 = (const utf32_char_t*)utf32_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf32_char_t *const src32x = (const utf32_char_t*)utf32_le_be[!!*(const char*)&le] + 1/*BOM*/;
	NULLSZ(utf16_to_utf32_size)
	NULLSZ(utf16u_to_utf32_size)
	NULLSZ(utf16x_to_utf32_size)
	NULLSZ(utf16ux_to_utf32_size)
	TEST(utf16_is_bom_be(utf16_le_be[1][0], utf16_le_be[1][1]));
	TEST(utf16_is_bom_le(utf16_le_be[0][0], utf16_le_be[0][1]));
	TEST(utf32_is_bom_be(utf32_le_be[1][0], utf32_le_be[1][1], utf32_le_be[1][2], utf32_le_be[1][3]));
	TEST(utf32_is_bom_le(utf32_le_be[0][0], utf32_le_be[0][1], utf32_le_be[0][2], utf32_le_be[0][3]));
	TESTSZ(SZ, utf16_to_utf32_size, utf16_char_t, src16, src16_e, utf32_sz - 1/*BOM*/)
	TESTSZ(SZ, utf16u_to_utf32_size, utf16_char_unaligned_t, src16u, src16u_e, utf32_sz - 1/*BOM*/)
	TESTSZ(SZ, utf16x_to_utf32_size, utf16_char_t, src16x, src16x_e, utf32_sz - 1/*BOM*/)
	TESTSZ(SZ, utf16ux_to_utf32_size, utf16_char_unaligned_t, src16ux, src16ux_e, utf32_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf16_to_utf32_z_size, utf16_char_t, src16, src16_e, utf32_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf16u_to_utf32_z_size, utf16_char_unaligned_t, src16u, src16u_e, utf32_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf16x_to_utf32_z_size, utf16_char_t, src16x, src16x_e, utf32_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf16ux_to_utf32_z_size, utf16_char_unaligned_t, src16ux, src16ux_e, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16_to_utf32, utf16_char_t, src16, src16_e, z(), utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16_to_utf32u, utf16_char_t, src16, src16_e, z(), utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16_to_utf32x, utf16_char_t, src16, src16_e, z(), utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16_to_utf32ux, utf16_char_t, src16, src16_e, z(), utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16u_to_utf32, utf16_char_unaligned_t, src16u, src16u_e, z(), utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16u_to_utf32u, utf16_char_unaligned_t, src16u, src16u_e, z(), utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16u_to_utf32x, utf16_char_unaligned_t, src16u, src16u_e, z(), utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16u_to_utf32ux, utf16_char_unaligned_t, src16u, src16u_e, z(), utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16x_to_utf32, utf16_char_t, src16x, src16x_e, z(), utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16x_to_utf32u, utf16_char_t, src16x, src16x_e, z(), utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16x_to_utf32x, utf16_char_t, src16x, src16x_e, z(), utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16x_to_utf32ux, utf16_char_t, src16x, src16x_e, z(), utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16ux_to_utf32, utf16_char_unaligned_t, src16ux, src16ux_e, z(), utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16ux_to_utf32u, utf16_char_unaligned_t, src16ux, src16ux_e, z(), utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16ux_to_utf32x, utf16_char_unaligned_t, src16ux, src16ux_e, z(), utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16ux_to_utf32ux, utf16_char_unaligned_t, src16ux, src16ux_e, z(), utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16_to_utf32_z, utf16_char_t, src16, src16_e, nz(), utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16_to_utf32u_z, utf16_char_t, src16, src16_e, nz(), utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16_to_utf32x_z, utf16_char_t, src16, src16_e, nz(), utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16_to_utf32ux_z, utf16_char_t, src16, src16_e, nz(), utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16u_to_utf32_z, utf16_char_unaligned_t, src16u, src16u_e, nz(), utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16u_to_utf32u_z, utf16_char_unaligned_t, src16u, src16u_e, nz(), utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16u_to_utf32x_z, utf16_char_unaligned_t, src16u, src16u_e, nz(), utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16u_to_utf32ux_z, utf16_char_unaligned_t, src16u, src16u_e, nz(), utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16x_to_utf32_z, utf16_char_t, src16x, src16x_e, nz(), utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16x_to_utf32u_z, utf16_char_t, src16x, src16x_e, nz(), utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16x_to_utf32x_z, utf16_char_t, src16x, src16x_e, nz(), utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16x_to_utf32ux_z, utf16_char_t, src16x, src16x_e, nz(), utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16ux_to_utf32_z, utf16_char_unaligned_t, src16ux, src16ux_e, nz(), utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16ux_to_utf32u_z, utf16_char_unaligned_t, src16ux, src16ux_e, nz(), utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16ux_to_utf32x_z, utf16_char_unaligned_t, src16ux, src16ux_e, nz(), utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16ux_to_utf32ux_z, utf16_char_unaligned_t, src16ux, src16ux_e, nz(), utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	return 0;
}

static int test_utf32_to_utf16(
	const unsigned initial_step,
	const unsigned char *const utf32_le_be[2],
	const unsigned char *const utf16_le_be[2],
	const unsigned utf32_sz,
	const unsigned utf16_sz,
	const unsigned utf16_buf_sz/*>=utf16_sz*/,
	utf16_char_t utf16_buf[/*utf16_buf_sz*/])
{
	const unsigned le = 1;
	const utf32_char_t *const src32 = (const utf32_char_t*)utf32_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf32_char_t *const src32_e = src32 + utf32_sz - 1/*BOM*/;
	const utf32_char_unaligned_t *const src32u = (const utf32_char_unaligned_t*)utf32_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf32_char_unaligned_t *const src32u_e = src32u + utf32_sz - 1/*BOM*/;
	const utf32_char_t *const src32x = (const utf32_char_t*)utf32_le_be[!!*(const char*)&le] + 1/*BOM*/;
	const utf32_char_t *const src32x_e = src32x + utf32_sz - 1/*BOM*/;
	const utf32_char_unaligned_t *const src32ux = (const utf32_char_unaligned_t*)utf32_le_be[!!*(const char*)&le] + 1/*BOM*/;
	const utf32_char_unaligned_t *const src32ux_e = src32ux + utf32_sz - 1/*BOM*/;
	const utf16_char_t *const src16 = (const utf16_char_t*)utf16_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf16_char_t *const src16x = (const utf16_char_t*)utf16_le_be[!!*(const char*)&le] + 1/*BOM*/;
	NULLSZ(utf32_to_utf16_size)
	NULLSZ(utf32u_to_utf16_size)
	NULLSZ(utf32x_to_utf16_size)
	NULLSZ(utf32ux_to_utf16_size)
	TEST(utf16_is_bom_be(utf16_le_be[1][0], utf16_le_be[1][1]));
	TEST(utf16_is_bom_le(utf16_le_be[0][0], utf16_le_be[0][1]));
	TEST(utf32_is_bom_be(utf32_le_be[1][0], utf32_le_be[1][1], utf32_le_be[1][2], utf32_le_be[1][3]));
	TEST(utf32_is_bom_le(utf32_le_be[0][0], utf32_le_be[0][1], utf32_le_be[0][2], utf32_le_be[0][3]));
	TESTSZ(SZ, utf32_to_utf16_size, utf32_char_t, src32, src32_e, utf16_sz - 1/*BOM*/)
	TESTSZ(SZ, utf32u_to_utf16_size, utf32_char_unaligned_t, src32u, src32u_e, utf16_sz - 1/*BOM*/)
	TESTSZ(SZ, utf32x_to_utf16_size, utf32_char_t, src32x, src32x_e, utf16_sz - 1/*BOM*/)
	TESTSZ(SZ, utf32ux_to_utf16_size, utf32_char_unaligned_t, src32ux, src32ux_e, utf16_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf32_to_utf16_z_size, utf32_char_t, src32, src32_e, utf16_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf32u_to_utf16_z_size, utf32_char_unaligned_t, src32u, src32u_e, utf16_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf32x_to_utf16_z_size, utf32_char_t, src32x, src32x_e, utf16_sz - 1/*BOM*/)
	TESTSZ(EMPTY, utf32ux_to_utf16_z_size, utf32_char_unaligned_t, src32ux, src32ux_e, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32_to_utf16, utf32_char_t, src32, src32_e, z(), utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32_to_utf16u, utf32_char_t, src32, src32_e, z(), utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32_to_utf16x, utf32_char_t, src32, src32_e, z(), utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32_to_utf16ux, utf32_char_t, src32, src32_e, z(), utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32u_to_utf16, utf32_char_unaligned_t, src32u, src32u_e, z(), utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32u_to_utf16u, utf32_char_unaligned_t, src32u, src32u_e, z(), utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32u_to_utf16x, utf32_char_unaligned_t, src32u, src32u_e, z(), utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32u_to_utf16ux, utf32_char_unaligned_t, src32u, src32u_e, z(), utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32x_to_utf16, utf32_char_t, src32x, src32x_e, z(), utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32x_to_utf16u, utf32_char_t, src32x, src32x_e, z(), utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32x_to_utf16x, utf32_char_t, src32x, src32x_e, z(), utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32x_to_utf16ux, utf32_char_t, src32x, src32x_e, z(), utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32ux_to_utf16, utf32_char_unaligned_t, src32ux, src32ux_e, z(), utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32ux_to_utf16u, utf32_char_unaligned_t, src32ux, src32ux_e, z(), utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32ux_to_utf16x, utf32_char_unaligned_t, src32ux, src32ux_e, z(), utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32ux_to_utf16ux, utf32_char_unaligned_t, src32ux, src32ux_e, z(), utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32_to_utf16_z, utf32_char_t, src32, src32_e, nz(), utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32_to_utf16u_z, utf32_char_t, src32, src32_e, nz(), utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32_to_utf16x_z, utf32_char_t, src32, src32_e, nz(), utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32_to_utf16ux_z, utf32_char_t, src32, src32_e, nz(), utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32u_to_utf16_z, utf32_char_unaligned_t, src32u, src32u_e, nz(), utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32u_to_utf16u_z, utf32_char_unaligned_t, src32u, src32u_e, nz(), utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32u_to_utf16x_z, utf32_char_unaligned_t, src32u, src32u_e, nz(), utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32u_to_utf16ux_z, utf32_char_unaligned_t, src32u, src32u_e, nz(), utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32x_to_utf16_z, utf32_char_t, src32x, src32x_e, nz(), utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32x_to_utf16u_z, utf32_char_t, src32x, src32x_e, nz(), utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32x_to_utf16x_z, utf32_char_t, src32x, src32x_e, nz(), utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32x_to_utf16ux_z, utf32_char_t, src32x, src32x_e, nz(), utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32ux_to_utf16_z, utf32_char_unaligned_t, src32ux, src32ux_e, nz(), utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32ux_to_utf16u_z, utf32_char_unaligned_t, src32ux, src32ux_e, nz(), utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32ux_to_utf16x_z, utf32_char_unaligned_t, src32ux, src32ux_e, nz(), utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32ux_to_utf16ux_z, utf32_char_unaligned_t, src32ux, src32ux_e, nz(), utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	return 0;
}

static int test_utf16_to_utf8_one(
	const unsigned utf16_sz,
	const unsigned utf8_sz,
	const unsigned char *const utf16_le_be[2],
	const utf8_char_t utf8[/*utf8_sz*/])
{
	const unsigned le = 1;
	unsigned i = 0;
	unsigned j = 0;
	utf8_state_t state = 0;
	const utf16_char_t *const src16 = (const utf16_char_t*)utf16_le_be[!*(const char*)&le] + 1/*BOM*/;
	for (; i < utf16_sz - 1/*BOM*/; i++) {
		utf8_char_t buf[UTF8_MAX_LEN + 5];
		unsigned q = UTF8_MAX_LEN;
		for (; q < sizeof(buf); q++)
			buf[q] = (utf8_char_t)(q + 10);
		{
			const utf16_char_t w = src16[i];
			const size_t sz = utf16_to_utf8_one(buf, w, &state);
			TEST(sz != (size_t)-1);
			if (sz) {
				TEST(sz <= utf8_sz - j);
				TEST(!memcmp(buf, &utf8[j], sz));
				j += (unsigned)sz;
			}
		}
		for (q = UTF8_MAX_LEN; q < sizeof(buf); q++)
			TEST(buf[q] == (utf8_char_t)(q + 10));
	}
	TEST(j == utf8_sz);
	return 0;
}

static int test_utf32_to_utf8_one(
	const unsigned utf32_sz,
	const unsigned utf8_sz,
	const unsigned char *const utf32_le_be[2],
	const utf8_char_t utf8[/*utf8_sz*/])
{
	const unsigned le = 1;
	unsigned i = 0;
	unsigned j = 0;
	const utf32_char_t *const src32 = (const utf32_char_t*)utf32_le_be[!*(const char*)&le] + 1/*BOM*/;
	for (; i < utf32_sz - 1/*BOM*/; i++) {
		utf8_char_t buf[UTF8_MAX_LEN + 5];
		unsigned q = UTF8_MAX_LEN;
		for (; q < sizeof(buf); q++)
			buf[q] = (utf8_char_t)(q + 10);
		{
			const utf32_char_t w = src32[i];
			const size_t sz = utf32_to_utf8_one(buf, w);
			TEST(sz != (size_t)-1);
			TEST(sz);
			TEST(sz <= utf8_sz - j);
			TEST(!memcmp(buf, &utf8[j], sz));
			j += (unsigned)sz;
		}
		for (q = UTF8_MAX_LEN; q < sizeof(buf); q++)
			TEST(buf[q] == (utf8_char_t)(q + 10));
	}
	TEST(j == utf8_sz);
	return 0;
}

static int test_utf8_to_utf16_one(
	const unsigned utf16_sz,
	const unsigned utf8_sz,
	const unsigned char *const utf16_le_be[2],
	const utf8_char_t utf8[/*utf8_sz*/])
{
	const unsigned le = 1;
	utf8_state_t state = 0;
	const utf16_char_t *const src16 = (const utf16_char_t*)utf16_le_be[!*(const char*)&le] + 1/*BOM*/;
	unsigned by = 0;
	for (; by < 15; by++) {
		unsigned i = 0;
		unsigned j = 0;
		unsigned y = by;
		while (i < utf8_sz) {
			utf16_char_t w;
			const unsigned n = (y <= utf8_sz - i) ? y : (utf8_sz - i);
			const size_t sz = utf8_to_utf16_one(&w, &utf8[i], n, &state);
			TEST(sz != (size_t)-1);
			if (sz == (size_t)-2) {
				const size_t sz_incomplete = utf8_to_utf16_one(&w, &utf8[i], 0, &state);
				TEST(sz_incomplete == (size_t)-2);
				TEST(i + n < utf8_sz);
				if (n)
					i += n;
				else
					y = 1;
				continue;
			}
			if (sz != (size_t)-3) {
				if (sz) {
					TEST(w);
					TEST(sz <= n);
					TEST(sz <= utf8_sz - i);
					i += (unsigned)sz;
				}
				else {
					TEST(!w);
					i++;
				}
			}
			else
				TEST(w);
			TEST(j < utf16_sz - 1/*BOM*/);
			TEST(w == src16[j]);
			j++;
			y = by;
		}
		TEST(i == utf8_sz);
		TEST(j == utf16_sz - 1/*BOM*/);
	}
	return 0;
}

static int test_utf8_to_utf32_one(
	const unsigned utf32_sz,
	const unsigned utf8_sz,
	const unsigned char *const utf32_le_be[2],
	const utf8_char_t utf8[/*utf8_sz*/])
{
	const unsigned le = 1;
	utf8_state_t state = 0;
	const utf32_char_t *const src32 = (const utf32_char_t*)utf32_le_be[!*(const char*)&le] + 1/*BOM*/;
	unsigned by = 0;
	for (; by < 15; by++) {
		unsigned i = 0;
		unsigned j = 0;
		unsigned y = by;
		while (i < utf8_sz) {
			utf32_char_t w;
			const unsigned n = (y <= utf8_sz - i) ? y : (utf8_sz - i);
			const size_t sz = utf8_to_utf32_one(&w, &utf8[i], n, &state);
			TEST(sz != (size_t)-1);
			if (sz == (size_t)-2) {
				const size_t sz_incomplete = utf8_to_utf32_one(&w, &utf8[i], 0, &state);
				TEST(sz_incomplete == (size_t)-2);
				TEST(i + n < utf8_sz);
				if (n)
					i += n;
				else
					y = 1;
				continue;
			}
			if (sz) {
				TEST(w);
				TEST(sz <= n);
				TEST(sz <= utf8_sz - i);
				i += (unsigned)sz;
			}
			else {
				TEST(!w);
				i++;
			}
			TEST(j < utf32_sz - 1/*BOM*/);
			TEST(w == src32[j]);
			j++;
			y = by;
		}
		TEST(i == utf8_sz);
		TEST(j == utf32_sz - 1/*BOM*/);
	}
	return 0;
}

static int test_utf8_len_one(
	const unsigned utf32_sz,
	const unsigned utf8_sz,
	const utf8_char_t utf8[/*utf8_sz*/])
{
	utf8_state_t state = 0;
	unsigned by = 0;
	for (; by < 15; by++) {
		unsigned j = 0;
		unsigned i = 0;
		unsigned y = by;
		while (i < utf8_sz) {
			const unsigned n = (y <= utf8_sz - i) ? y : (utf8_sz - i);
			const size_t sz = utf8_len_one(&utf8[i], n, &state);
			TEST(sz != (size_t)-1);
			if (sz == (size_t)-2) {
				TEST(i + n < utf8_sz);
				if (n)
					i += n;
				else
					y = 1;
				continue;
			}
			if (sz) {
				TEST(utf8[i]);
				TEST(sz <= n);
				TEST(sz <= utf8_sz - i);
				i += (unsigned)sz;
			}
			else {
				TEST(!utf8[i]);
				i++;
			}
			j++;
			y = by;
		}
		TEST(i == utf8_sz);
		TEST(j == utf32_sz - 1/*BOM*/);
	}
	return 0;
}

static int test_utf8_to_utf32_one_z(
	const unsigned utf32_sz,
	const unsigned utf8_sz,
	const unsigned char *const utf32_le_be[2],
	const utf8_char_t utf8[/*utf8_sz*/])
{
	const unsigned le = 1;
	const utf32_char_t *const src32 = (const utf32_char_t*)utf32_le_be[!*(const char*)&le] + 1/*BOM*/;
	const utf8_char_t *p = utf8;
	const utf8_char_t *const e = utf8 + utf8_sz;
	unsigned j = 0;
	while (p < e) {
		utf32_char_t w;
		const utf8_char_t *const n = utf8_to_utf32_one_z(&w, p);
		TEST(n);
		TEST(n <= e);
		TEST(j < utf32_sz - 1/*BOM*/);
		TEST(w == src32[j]);
		j++;
		p = n;
	}
	TEST(p == e);
	TEST(j == utf32_sz - 1/*BOM*/);
	return 0;
}

static int test_utf8_mblen(
	const unsigned utf32_sz,
	const unsigned utf8_sz,
	const utf8_char_t utf8[/*utf8_sz*/])
{
	errno = 0;
	TEST(0 == utf8_mblen(NULL, 0));
	TEST(!errno);
	TEST(0 == utf8_mblen(NULL, 1));
	TEST(!errno);
#ifdef CHECK_UTF8_LOCALE
	LIBC_TEST(0 == mblen(NULL, 0));
	LIBC_TEST(!errno);
	LIBC_TEST(0 == mblen(NULL, 1));
	LIBC_TEST(!errno);
#endif
	{
		unsigned by = 0;
		for (; by < 15; by++) {
			unsigned j = 0;
			unsigned i = 0;
			while (i < utf8_sz) {
				unsigned n = (by <= utf8_sz - i) ? by : (utf8_sz - i);
				for (;; n++) {
					const int sz = utf8_mblen(&utf8[i], n);
					TEST(!errno);
#ifdef CHECK_UTF8_LOCALE
					{
						const int sz1 = mblen((const char*)&utf8[i], n);
#ifdef LIBC_UCRT /* if n == 0, ucrt checks (*s) and always returns 0 */
						if (n)
#endif
							LIBC_TEST(sz == sz1);
						LIBC_TEST(!errno);
					}
#endif
					if (sz != -1) {
						if (sz) {
							TEST(utf8[i]);
							TEST((unsigned)sz <= n);
							TEST((unsigned)sz <= utf8_sz - i);
							i += (unsigned)sz;
						}
						else {
							TEST(!utf8[i]);
							i++;
						}
						break;
					}
					TEST(i + n < utf8_sz);
					TEST(-1 == utf8_mblen(&utf8[i], 0));
					TEST(!errno);
					TEST(0 == utf8_mblen(NULL, 0));
					TEST(!errno);
					TEST(0 == utf8_mblen(NULL, 1));
					TEST(!errno);
#ifdef CHECK_UTF8_LOCALE
#ifndef LIBC_UCRT /* if n == 0, ucrt checks (*s) and always returns 0 */
					LIBC_TEST(-1 == mblen((const char*)&utf8[i], 0));
#endif
					LIBC_TEST(!errno);
					LIBC_TEST(0 == mblen(NULL, 0));
					LIBC_TEST(!errno);
					LIBC_TEST(0 == mblen(NULL, 1));
					LIBC_TEST(!errno);
#endif
				}
				j++;
			}
			TEST(i == utf8_sz);
			TEST(j == utf32_sz - 1/*BOM*/);
		}
	}
	return 0;
}

static int test_utf8_mbrlen(
	const unsigned utf32_sz,
	const unsigned utf8_sz,
	const utf8_char_t utf8[/*utf8_sz*/])
{
	errno = 0;
	TEST(0 == utf8_mbrlen(NULL, 0, NULL));
	TEST(!errno);
	TEST(0 == utf8_mbrlen(NULL, 1, NULL));
	TEST(!errno);
#ifdef CHECK_UTF8_LOCALE
	LIBC_TEST(0 == mbrlen(NULL, 0, NULL));
	LIBC_TEST(!errno);
	LIBC_TEST(0 == mbrlen(NULL, 1, NULL));
	LIBC_TEST(!errno);
#endif
	{
		unsigned by = 0;
		utf8_state_t state = 0;
#ifdef CHECK_UTF8_LOCALE
		mbstate_t state1;
		memset(&state1, 0, sizeof(state1));
#endif
		for (; by < 15; by++) {
			unsigned j = 0;
			unsigned i = 0;
			unsigned y = by;
			while (i < utf8_sz) {
				unsigned n = (y <= utf8_sz - i) ? y : (utf8_sz - i);
				const size_t sz = utf8_mbrlen(&utf8[i], n, &state);
				TEST(!errno);
				TEST(sz != (size_t)-1);
#ifdef CHECK_UTF8_LOCALE
				{
					const size_t sz1 = mbrlen((const char*)&utf8[i], n, &state1);
#ifdef LIBC_UCRT /* if n == 0, ucrt always returns 0, even if state is not initial  */
					if (n)
#endif
						LIBC_TEST(sz == sz1);
					LIBC_TEST(!errno);
				}
#endif
				if (sz == (size_t)-2) {
					unsigned t = 0;
					const utf8_state_t saved_state = state;
#ifdef CHECK_UTF8_LOCALE
					mbstate_t saved_state1;
					memcpy(&saved_state1, &state1, sizeof(state1));
#endif
					TEST(i + n < utf8_sz);
					for (; t < 2; t++) {
						const size_t sz_incomplete = utf8_mbrlen(&utf8[i], 0, &state);
						TEST(!errno);
						TEST(sz_incomplete == (size_t)-2);
						TEST(state == saved_state);
#ifdef CHECK_UTF8_LOCALE
						{
							const size_t sz_incomplete1 = mbrlen((const char*)&utf8[i], 0, &state1);
#ifdef LIBC_UCRT /* if n == 0, ucrt always returns 0, even if state is not initial */
							(void)sz_incomplete1;
#else
							LIBC_TEST(sz_incomplete == sz_incomplete1);
#endif
							LIBC_TEST(!errno);
							LIBC_TEST(!memcmp(&state1, &saved_state1, sizeof(state1)));
						}
#endif
					}
					for (t = 0; t < 2; t++) {
						const size_t sz_incomplete = utf8_mbrlen(NULL, t, &state);
						TEST(state == saved_state);
						if (!saved_state) {
							TEST(sz_incomplete == 0);
							TEST(!errno);
						}
						else {
							TEST(sz_incomplete == (size_t)-1);
							TEST(errno);
							errno = 0;
						}
#ifdef CHECK_UTF8_LOCALE
						{
							const size_t sz_incomplete1 = mbrlen(NULL, t, &state1);
#ifdef LIBC_UCRT /* if s == NULL, ucrt always returns 0, even if state is not initial */
							(void)sz_incomplete1;
#else
							LIBC_TEST(sz_incomplete == sz_incomplete1);
#endif
							LIBC_TEST(!memcmp(&state1, &saved_state1, sizeof(state1)));
							if (sz_incomplete == 0)
								LIBC_TEST(!errno);
							else {
#ifndef LIBC_UCRT /* if s == NULL, ucrt always returns 0, even if state is not initial */
								LIBC_TEST(errno);
								errno = 0;
#endif
							}
						}
#endif
					}
					if (n)
						i += n;
					else
						y = 1;
					continue;
				}
				TEST(n);
				if (sz) {
					TEST(utf8[i]);
					TEST(sz <= n);
					TEST(sz <= utf8_sz - i);
					i += (unsigned)sz;
				}
				else {
					TEST(!utf8[i]);
					TEST(n == 1);
					i++;
				}
				j++;
				y = by;
			}
			TEST(i == utf8_sz);
			TEST(j == utf32_sz - 1/*BOM*/);
		}
	}
	return 0;
}

static int test_utf8_mbrtoc16_or_32(
	const unsigned utf16_or_32_sz,
	const unsigned utf8_sz,
	const unsigned char *const utf16_or_32_le_be[2],
	const utf8_char_t utf8[/*utf8_sz*/],
	const int check_16)
{
	const unsigned le = 1;
	const union {
		const unsigned char *src;
		const utf16_char_t *src16;
		const utf32_char_t *src32;
	} src = {utf16_or_32_le_be[!*(const char*)&le] + (check_16 ? sizeof(utf16_char_t) : sizeof(utf32_char_t))/*BOM*/};
	unsigned round = 0; /* will check with pwc == NULL on second round */
	errno = 0;
	for (; round < 2; round++) {
		utf8_state_t state = 0;
		unsigned by = 0;
#ifdef CHECK_UTF8_LOCALE
		mbstate_t state1;
		memset(&state1, 0, sizeof(state1));
#endif
		if (round) {
			if (check_16) {
				TEST(0 == utf8_mbrtoc16(NULL, NULL, 0, NULL));
				TEST(!errno);
				TEST(0 == utf8_mbrtoc16(NULL, NULL, 1, NULL));
				TEST(!errno);
#ifdef CHECK_UTF8_LOCALE
				LIBC_TEST(0 == mbrtoc16(NULL, NULL, 0, NULL));
				LIBC_TEST(!errno);
				LIBC_TEST(0 == mbrtoc16(NULL, NULL, 1, NULL));
				LIBC_TEST(!errno);
#endif
			}
			else {
				TEST(0 == utf8_mbrtoc32(NULL, NULL, 0, NULL));
				TEST(!errno);
				TEST(0 == utf8_mbrtoc32(NULL, NULL, 1, NULL));
				TEST(!errno);
#ifdef CHECK_UTF8_LOCALE
				LIBC_TEST(0 == mbrtoc32(NULL, NULL, 0, NULL));
				LIBC_TEST(!errno);
				LIBC_TEST(0 == mbrtoc32(NULL, NULL, 1, NULL));
				LIBC_TEST(!errno);
#endif
			}
		}
		for (; by < 15; by++) {
			unsigned i = 0;
			unsigned j = 0;
			unsigned y = by;
			while (i < utf8_sz) {
				const unsigned n = (y <= utf8_sz - i) ? y : (utf8_sz - i);
				union {
					utf32_char_t w32;
					utf16_char_t w16;
				} dst = {0};
#ifdef CHECK_UTF8_LOCALE
				union {
					char32_t w32;
					char16_t w16;
				} dst1 = {0};
#endif
				const size_t sz = check_16 ?
					utf8_mbrtoc16(round ? NULL : &dst.w16, &utf8[i], n, &state) :
					utf8_mbrtoc32(round ? NULL : &dst.w32, &utf8[i], n, &state);
				TEST(!errno);
				TEST(sz != (size_t)-1);
#ifdef CHECK_UTF8_LOCALE
				{
					const size_t sz1 = check_16 ?
						mbrtoc16(round ? NULL : &dst1.w16, (const char*)&utf8[i], n, &state1) :
						mbrtoc32(round ? NULL : &dst1.w32, (const char*)&utf8[i], n, &state1);
					LIBC_TEST(sz == sz1);
					LIBC_TEST(!errno);
				}
#endif
				if (sz == (size_t)-2) {
					unsigned t = 0;
					const utf8_state_t saved_state = state;
#ifdef CHECK_UTF8_LOCALE
					mbstate_t saved_state1;
					memcpy(&saved_state1, &state1, sizeof(state1));
#endif
					TEST(i + n < utf8_sz);
					for (; t < 2; t++) {
						const size_t sz_incomplete = check_16 ?
							utf8_mbrtoc16(round ? NULL : &dst.w16, &utf8[i], 0, &state) :
							utf8_mbrtoc32(round ? NULL : &dst.w32, &utf8[i], 0, &state);
						TEST(!errno);
						TEST(sz_incomplete == (size_t)-2);
						TEST(state == saved_state);
#ifdef CHECK_UTF8_LOCALE
						{
							const size_t sz_incomplete1 = check_16 ?
								mbrtoc16(round ? NULL : &dst1.w16, (const char*)&utf8[i], 0, &state1) :
								mbrtoc32(round ? NULL : &dst1.w32, (const char*)&utf8[i], 0, &state1);
							LIBC_TEST(sz_incomplete == sz_incomplete1);
							LIBC_TEST(!errno);
							LIBC_TEST(!memcmp(&state1, &saved_state1, sizeof(state1)));
						}
#endif
					}
					for (t = 0; t < 2; t++) {
						const size_t sz_incomplete = check_16 ?
							utf8_mbrtoc16(round ? NULL : &dst.w16, NULL, t, &state) :
							utf8_mbrtoc32(round ? NULL : &dst.w32, NULL, t, &state);
						TEST(state == saved_state);
						if (!saved_state) {
							TEST(sz_incomplete == 0);
							TEST(!errno);
						}
						else {
							TEST(sz_incomplete == (size_t)-1);
							TEST(errno);
							errno = 0;
						}
#ifdef CHECK_UTF8_LOCALE
						{
#ifdef LIBC_UCRT
							/* there is a bug in ucrt library: if state1 is not initial,
							  mbrtoc16(NULL, NULL, 0, &state1) is not equivalent (returns (size_t)-2) to
							  mbrtoc16(NULL, "", 1, &state1), which returns (size_t)-1 (which is correct),
							  the same bug for mbrtoc32() */
							const size_t sz_incomplete1 = check_16 ?
								mbrtoc16(round ? NULL : &dst1.w16, "", 1, &state1) :
								mbrtoc32(round ? NULL : &dst1.w32, "", 1, &state1);
#else
							const size_t sz_incomplete1 = check_16 ?
								mbrtoc16(round ? NULL : &dst1.w16, NULL, t, &state1) :
								mbrtoc32(round ? NULL : &dst1.w32, NULL, t, &state1);
#endif
							LIBC_TEST(sz_incomplete == sz_incomplete1);
#ifdef LIBC_UCRT /* ucrt resets state on empty string */
							if (sz_incomplete == (size_t)-1)
								memcpy(&state1, &saved_state1, sizeof(state1));
#endif
							LIBC_TEST(!memcmp(&state1, &saved_state1, sizeof(state1)));
							if (sz_incomplete == 0)
								LIBC_TEST(!errno);
							else {
								LIBC_TEST(errno);
								errno = 0;
							}
						}
#endif
					}
					if (n)
						i += n;
					else
						y = 1;
					continue;
				}
				if (!check_16 || sz != (size_t)-3) {
					TEST(n);
					if (sz) {
						if (!round) {
							if (check_16)
								TEST(dst.w16);
							else
								TEST(dst.w32);
						}
						TEST(sz <= n);
						TEST(sz <= utf8_sz - i);
						i += (unsigned)sz;
					}
					else {
						if (!round) {
							if (check_16)
								TEST(!dst.w16);
							else
								TEST(!dst.w32);
						}
						i++;
					}
				}
				else if (!round)
					TEST(dst.w16); /* second surrogate */
				TEST(j < utf16_or_32_sz - 1/*BOM*/);
				if (!round) {
					if (check_16)
						TEST(dst.w16 == src.src16[j]);
					else
						TEST(dst.w32 == src.src32[j]);
#ifdef CHECK_UTF8_LOCALE
					{
						if (check_16)
							LIBC_TEST(dst.w16 == dst1.w16);
						else
							LIBC_TEST(dst.w32 == dst1.w32);
					}
#endif
				}
				j++;
				y = by;
			}
			TEST(i == utf8_sz);
			TEST(j == utf16_or_32_sz - 1/*BOM*/);
		}
	}
	return 0;
}

static int test_utf8_c16_or_32rtomb(
	const unsigned utf16_or_32_sz,
	const unsigned utf8_sz,
	const unsigned char *const utf16_or_32_le_be[2],
	const utf8_char_t utf8[/*utf8_sz*/],
	const int check_16)
{
	const unsigned le = 1;
	const union {
		const unsigned char *src;
		const utf16_char_t *src16;
		const utf32_char_t *src32;
	} src = {utf16_or_32_le_be[!*(const char*)&le] + (check_16 ? sizeof(utf16_char_t) : sizeof(utf32_char_t))/*BOM*/};
	errno = 0;
	if (check_16) {
		TEST(1 == utf8_c16rtomb(NULL, 0, NULL));
		TEST(!errno);
		TEST(1 == utf8_c16rtomb(NULL, 1, NULL));
		TEST(!errno);
#ifdef CHECK_UTF8_LOCALE
		LIBC_TEST(1 == c16rtomb(NULL, 0, NULL));
		LIBC_TEST(!errno);
		LIBC_TEST(1 == c16rtomb(NULL, 1, NULL));
		LIBC_TEST(!errno);
#endif
	}
	else {
		TEST(1 == utf8_c32rtomb(NULL, 0, NULL));
		TEST(!errno);
		TEST(1 == utf8_c32rtomb(NULL, 1, NULL));
		TEST(!errno);
#ifdef CHECK_UTF8_LOCALE
#ifndef LIBC_UCRT /* ucrt crashes is state pointer is NULL */
		LIBC_TEST(1 == c32rtomb(NULL, 0, NULL));
		LIBC_TEST(!errno);
		LIBC_TEST(1 == c32rtomb(NULL, 1, NULL));
		LIBC_TEST(!errno);
#endif
#endif
	}
	{
		unsigned j = 0;
		unsigned i = 0;
		utf8_state_t state = 0;
#ifdef CHECK_UTF8_LOCALE
		mbstate_t state1;
		memset(&state1, 0, sizeof(state1));
#endif
		while (i < utf16_or_32_sz - 1/*BOM*/) {
			utf8_char_t s[UTF8_MAX_LEN] = {0};
#ifdef CHECK_UTF8_LOCALE
			char s1[UTF8_MAX_LEN] = {0};
#endif
			const size_t sz = check_16 ?
				utf8_c16rtomb(s, src.src16[i], &state) :
				utf8_c32rtomb(s, src.src32[i], &state);
			TEST(!errno);
			TEST(sz != (size_t)-1);
#ifdef CHECK_UTF8_LOCALE
			{
				const size_t sz1 = check_16 ?
					c16rtomb(s1, (char16_t)src.src16[i], &state1) :
					c32rtomb(s1, (char32_t)src.src32[i], &state1);
				LIBC_TEST(sz == sz1);
				LIBC_TEST(!errno);
			}
#endif
			TEST(sz || check_16);
			if (!sz) {
				const utf8_state_t saved_state = state;
#ifdef CHECK_UTF8_LOCALE
				mbstate_t saved_state1;
				memcpy(&saved_state1, &state1, sizeof(state1));
#endif
				TEST(i + 1 < utf16_or_32_sz - 1/*BOM*/);
				{
					const size_t sz_incomplete = utf8_c16rtomb(s, 0, &state);
					TEST(sz_incomplete == (size_t)-1);
					TEST(errno);
					TEST(state == saved_state);
					errno = 0;
#ifdef CHECK_UTF8_LOCALE
					{
						const size_t sz_incomplete1 = c16rtomb(s1, 0, &state1);
						LIBC_TEST(sz_incomplete == sz_incomplete1);
						LIBC_TEST(errno);
#ifdef LIBC_UCRT /* ucrt resets state on invalid input */
						if (sz_incomplete == (size_t)-1)
							memcpy(&state1, &saved_state1, sizeof(state1));
#endif
						LIBC_TEST(!memcmp(&state1, &saved_state1, sizeof(state1)));
						errno = 0;
					}
#endif
				}
				{
					const size_t sz_incomplete = utf8_c16rtomb(NULL, 1, &state);
					TEST(sz_incomplete == (size_t)-1);
					TEST(errno);
					TEST(state == saved_state);
					errno = 0;
#ifdef CHECK_UTF8_LOCALE
					{
						const size_t sz_incomplete1 = c16rtomb(NULL, 1, &state1);
						LIBC_TEST(sz_incomplete == sz_incomplete1);
						LIBC_TEST(errno);
#ifdef LIBC_UCRT /* ucrt resets state on invalid input */
						if (sz_incomplete == (size_t)-1)
							memcpy(&state1, &saved_state1, sizeof(state1));
#endif
						LIBC_TEST(!memcmp(&state1, &saved_state1, sizeof(state1)));
						errno = 0;
					}
#endif
				}
				i += 1;
				continue;
			}
			TEST(sz <= sizeof(s));
			TEST(j + sz <= utf8_sz);
			TEST(!memcmp(s, &utf8[j], sz));
#ifdef CHECK_UTF8_LOCALE
			TEST(!memcmp(s, s1, sz));
#endif
			{
				const size_t sz_incomplete = check_16 ?
					utf8_c16rtomb(s, 0, &state) :
					utf8_c32rtomb(s, 0, &state);
				TEST(sz_incomplete == 1);
				TEST(!errno);
#ifdef CHECK_UTF8_LOCALE
				{
					const size_t sz_incomplete1 = check_16 ?
						c16rtomb(s1, 0, &state1) :
						c32rtomb(s1, 0, &state1);
					LIBC_TEST(sz_incomplete == sz_incomplete1);
					LIBC_TEST(!errno);
				}
#endif
			}
			{
				const size_t sz_incomplete = check_16 ?
					utf8_c16rtomb(NULL, 1, &state) :
					utf8_c32rtomb(NULL, 1, &state);
				TEST(sz_incomplete == 1);
				TEST(!errno);
#ifdef CHECK_UTF8_LOCALE
				{
					const size_t sz_incomplete1 = check_16 ?
						c16rtomb(NULL, 1, &state1) :
						c32rtomb(NULL, 1, &state1);
					LIBC_TEST(sz_incomplete == sz_incomplete1);
					LIBC_TEST(!errno);
				}
#endif
			}
			i += 1;
			j += (unsigned)sz;
		}
		TEST(i == utf16_or_32_sz - 1/*BOM*/);
		TEST(j == utf8_sz);
	}
	return 0;
}

#include "test_data.inl"

int main(int argc, char *argv[])
{
#ifdef CHECK_UTF8_LOCALE
#ifdef _WIN32
	const char *utf8_locale = ".utf8";
#else
	const char *utf8_locale = "C.UTF-8";
#endif
	const char *const locale = setlocale(LC_ALL, utf8_locale);
	if (!locale) {
		fprintf(stderr, "failed to set locale: %s\n", utf8_locale);
		return -2;
	}
#endif /* CHECK_UTF8_LOCALE */
	fill_test_bufs();
	{
		union {
			utf16_char_t utf16_1_buf[sizeof(utf16_le_1)/sizeof(utf16_char_t) + 10];
			utf32_char_t utf32_1_buf[sizeof(utf32_le_1)/sizeof(utf32_char_t) + 10];
			utf8_char_t utf8_1_buf[sizeof(utf8_1) + 10];
			utf16_char_t utf16_2_buf[sizeof(utf16_le_2)/sizeof(utf16_char_t) + 10];
			utf32_char_t utf32_2_buf[sizeof(utf32_le_2)/sizeof(utf32_char_t) + 10];
			utf8_char_t utf8_2_buf[sizeof(utf8_2) + 10];
		} u;
		const struct {
			const unsigned initial_step;
			const unsigned char *const utf16_le_be[2];
			const unsigned char *const utf32_le_be[2];
			const utf8_char_t *utf8;
			utf16_char_t *utf16_buf;
			utf32_char_t *utf32_buf;
			utf8_char_t *utf8_buf;
			const unsigned utf16_sz;
			const unsigned utf32_sz;
			const unsigned utf8_sz;
		} data[] = {
			{
				100,
				{utf16_le_1, utf16_be_1},
				{utf32_le_1, utf32_be_1},
				utf8_1,
				u.utf16_1_buf,
				u.utf32_1_buf,
				u.utf8_1_buf,
				sizeof(utf16_le_1)/sizeof(utf16_char_t),
				sizeof(utf32_le_1)/sizeof(utf32_char_t),
				sizeof(utf8_1)
			},
			{
				1,
				{utf16_le_2, utf16_be_2},
				{utf32_le_2, utf32_be_2},
				utf8_2,
				u.utf16_2_buf,
				u.utf32_2_buf,
				u.utf8_2_buf,
				sizeof(utf16_le_2)/sizeof(utf16_char_t),
				sizeof(utf32_le_2)/sizeof(utf32_char_t),
				sizeof(utf8_2)
			}
		};
		unsigned z = 0;
		for (; z < sizeof(data)/sizeof(data[0]); z++) {
			TEST(!test_utf16_to_utf8(
				data[z].initial_step,
				data[z].utf16_le_be,
				data[z].utf16_sz,
				data[z].utf8_sz,
				data[z].utf8,
				data[z].utf8_sz + 10,
				data[z].utf8_buf));
			TEST(!test_utf32_to_utf8(
				data[z].initial_step,
				data[z].utf32_le_be,
				data[z].utf32_sz,
				data[z].utf8_sz,
				data[z].utf8,
				data[z].utf8_sz + 10,
				data[z].utf8_buf));
			TEST(!test_utf8_to_utf16(
				data[z].initial_step,
				data[z].utf16_le_be,
				data[z].utf16_sz,
				data[z].utf8_sz,
				data[z].utf8,
				data[z].utf16_sz + 10,
				data[z].utf16_buf));
			TEST(!test_utf8_to_utf32(
				data[z].initial_step,
				data[z].utf32_le_be,
				data[z].utf32_sz,
				data[z].utf8_sz,
				data[z].utf8,
				data[z].utf32_sz + 10,
				data[z].utf32_buf));
			TEST(!test_utf16_to_utf32(
				data[z].initial_step,
				data[z].utf16_le_be,
				data[z].utf32_le_be,
				data[z].utf16_sz,
				data[z].utf32_sz,
				data[z].utf32_sz + 10,
				data[z].utf32_buf));
			TEST(!test_utf32_to_utf16(
				data[z].initial_step,
				data[z].utf32_le_be,
				data[z].utf16_le_be,
				data[z].utf32_sz,
				data[z].utf16_sz,
				data[z].utf16_sz + 10,
				data[z].utf16_buf));
			TEST(!test_utf16_to_utf8_one(
				data[z].utf16_sz,
				data[z].utf8_sz,
				data[z].utf16_le_be,
				data[z].utf8));
			TEST(!test_utf32_to_utf8_one(
				data[z].utf32_sz,
				data[z].utf8_sz,
				data[z].utf32_le_be,
				data[z].utf8));
			TEST(!test_utf8_to_utf16_one(
				data[z].utf16_sz,
				data[z].utf8_sz,
				data[z].utf16_le_be,
				data[z].utf8));
			TEST(!test_utf8_to_utf32_one(
				data[z].utf32_sz,
				data[z].utf8_sz,
				data[z].utf32_le_be,
				data[z].utf8));
			TEST(!test_utf8_len_one(
				data[z].utf32_sz,
				data[z].utf8_sz,
				data[z].utf8));
			TEST(!test_utf8_to_utf32_one_z(
				data[z].utf32_sz,
				data[z].utf8_sz,
				data[z].utf32_le_be,
				data[z].utf8));
			TEST(!test_utf8_mblen(
				data[z].utf32_sz,
				data[z].utf8_sz,
				data[z].utf8));
			TEST(!test_utf8_mbrlen(
				data[z].utf32_sz,
				data[z].utf8_sz,
				data[z].utf8));
			TEST(!test_utf8_mbrtoc16_or_32(
				data[z].utf16_sz,
				data[z].utf8_sz,
				data[z].utf16_le_be,
				data[z].utf8,
				1));
			TEST(!test_utf8_mbrtoc16_or_32(
				data[z].utf32_sz,
				data[z].utf8_sz,
				data[z].utf32_le_be,
				data[z].utf8,
				0));
			TEST(!test_utf8_c16_or_32rtomb(
				data[z].utf16_sz,
				data[z].utf8_sz,
				data[z].utf16_le_be,
				data[z].utf8,
				1));
			TEST(!test_utf8_c16_or_32rtomb(
				data[z].utf32_sz,
				data[z].utf8_sz,
				data[z].utf32_le_be,
				data[z].utf8,
				0));
		}
	}
	printf("All tests ok\n");
	(void)argc, (void)argv;
	return 0;
}
