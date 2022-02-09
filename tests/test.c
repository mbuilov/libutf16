/* test.c */

/* compile as:
  gcc tests/test.c -I. libutf16.a -o test
   or
  cl /Fetest.exe tests\test.c /I. /link utf16.a
*/

#include <stdio.h>
#include <stdint.h>
#include <memory.h>

#include "libutf16/utf16_to_utf8.h"
#include "libutf16/utf32_to_utf8.h"
#include "libutf16/utf8_to_utf16.h"
#include "libutf16/utf8_to_utf32.h"
#include "libutf16/utf16_to_utf32.h"
#include "libutf16/utf32_to_utf16.h"

static unsigned long long test_number = 0;

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
	TESTCONV(SZ, utf16_to_utf8, utf16_char_t, src16, src16_e, 0, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(SZ, utf16u_to_utf8, utf16_char_unaligned_t, src16u, src16u_e, 0, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(SZ, utf16x_to_utf8, utf16_char_t, src16x, src16x_e, 0, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(SZ, utf16ux_to_utf8, utf16_char_unaligned_t, src16ux, src16ux_e, 0, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf16_to_utf8_z, utf16_char_t, src16, src16_e, 1, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf16u_to_utf8_z, utf16_char_unaligned_t, src16u, src16u_e, 1, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf16x_to_utf8_z, utf16_char_t, src16x, src16x_e, 1, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf16ux_to_utf8_z, utf16_char_unaligned_t, src16ux, src16ux_e, 1, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
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
	TESTCONV(SZ, utf32_to_utf8, utf32_char_t, src32, src32_e, 0, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(SZ, utf32u_to_utf8, utf32_char_unaligned_t, src32u, src32u_e, 0, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(SZ, utf32x_to_utf8, utf32_char_t, src32x, src32x_e, 0, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(SZ, utf32ux_to_utf8, utf32_char_unaligned_t, src32ux, src32ux_e, 0, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf32_to_utf8_z, utf32_char_t, src32, src32_e, 1, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf32u_to_utf8_z, utf32_char_unaligned_t, src32u, src32u_e, 1, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf32x_to_utf8_z, utf32_char_t, src32x, src32x_e, 1, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
	TESTCONV(EMPTY, utf32ux_to_utf8_z, utf32_char_unaligned_t, src32ux, src32ux_e, 1, utf8_char_t, utf8_char_t, utf8, utf8_buf, utf8_buf_sz, utf8_sz)
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
	TESTCONV(SZ, utf8_to_utf16, utf8_char_t, utf8, utf8_e, 0, utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf16u, utf8_char_t, utf8, utf8_e, 0, utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf16x, utf8_char_t, utf8, utf8_e, 0, utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf16ux, utf8_char_t, utf8, utf8_e, 0, utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf16_z, utf8_char_t, utf8, utf8_e, 1, utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf16u_z, utf8_char_t, utf8, utf8_e, 1, utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf16x_z, utf8_char_t, utf8, utf8_e, 1, utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf16ux_z, utf8_char_t, utf8, utf8_e, 1, utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
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
	TESTCONV(SZ, utf8_to_utf32, utf8_char_t, utf8, utf8_e, 0, utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf32u, utf8_char_t, utf8, utf8_e, 0, utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf32x, utf8_char_t, utf8, utf8_e, 0, utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf8_to_utf32ux, utf8_char_t, utf8, utf8_e, 0, utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf32_z, utf8_char_t, utf8, utf8_e, 1, utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf32u_z, utf8_char_t, utf8, utf8_e, 1, utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf32x_z, utf8_char_t, utf8, utf8_e, 1, utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf8_to_utf32ux_z, utf8_char_t, utf8, utf8_e, 1, utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
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
	TESTCONV(SZ, utf16_to_utf32, utf16_char_t, src16, src16_e, 0, utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16_to_utf32u, utf16_char_t, src16, src16_e, 0, utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16_to_utf32x, utf16_char_t, src16, src16_e, 0, utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16_to_utf32ux, utf16_char_t, src16, src16_e, 0, utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16u_to_utf32, utf16_char_unaligned_t, src16u, src16u_e, 0, utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16u_to_utf32u, utf16_char_unaligned_t, src16u, src16u_e, 0, utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16u_to_utf32x, utf16_char_unaligned_t, src16u, src16u_e, 0, utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16u_to_utf32ux, utf16_char_unaligned_t, src16u, src16u_e, 0, utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16x_to_utf32, utf16_char_t, src16x, src16x_e, 0, utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16x_to_utf32u, utf16_char_t, src16x, src16x_e, 0, utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16x_to_utf32x, utf16_char_t, src16x, src16x_e, 0, utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16x_to_utf32ux, utf16_char_t, src16x, src16x_e, 0, utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16ux_to_utf32, utf16_char_unaligned_t, src16ux, src16ux_e, 0, utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16ux_to_utf32u, utf16_char_unaligned_t, src16ux, src16ux_e, 0, utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16ux_to_utf32x, utf16_char_unaligned_t, src16ux, src16ux_e, 0, utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(SZ, utf16ux_to_utf32ux, utf16_char_unaligned_t, src16ux, src16ux_e, 0, utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16_to_utf32_z, utf16_char_t, src16, src16_e, 1, utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16_to_utf32u_z, utf16_char_t, src16, src16_e, 1, utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16_to_utf32x_z, utf16_char_t, src16, src16_e, 1, utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16_to_utf32ux_z, utf16_char_t, src16, src16_e, 1, utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16u_to_utf32_z, utf16_char_unaligned_t, src16u, src16u_e, 1, utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16u_to_utf32u_z, utf16_char_unaligned_t, src16u, src16u_e, 1, utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16u_to_utf32x_z, utf16_char_unaligned_t, src16u, src16u_e, 1, utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16u_to_utf32ux_z, utf16_char_unaligned_t, src16u, src16u_e, 1, utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16x_to_utf32_z, utf16_char_t, src16x, src16x_e, 1, utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16x_to_utf32u_z, utf16_char_t, src16x, src16x_e, 1, utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16x_to_utf32x_z, utf16_char_t, src16x, src16x_e, 1, utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16x_to_utf32ux_z, utf16_char_t, src16x, src16x_e, 1, utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16ux_to_utf32_z, utf16_char_unaligned_t, src16ux, src16ux_e, 1, utf32_char_t, utf32_char_t, src32, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16ux_to_utf32u_z, utf16_char_unaligned_t, src16ux, src16ux_e, 1, utf32_char_t, utf32_char_unaligned_t, src32, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16ux_to_utf32x_z, utf16_char_unaligned_t, src16ux, src16ux_e, 1, utf32_char_t, utf32_char_t, src32x, utf32_buf, utf32_buf_sz, utf32_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf16ux_to_utf32ux_z, utf16_char_unaligned_t, src16ux, src16ux_e, 1, utf32_char_t, utf32_char_unaligned_t, src32x, ((utf32_char_unaligned_t*)utf32_buf), utf32_buf_sz, utf32_sz - 1/*BOM*/)
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
	TESTCONV(SZ, utf32_to_utf16, utf32_char_t, src32, src32_e, 0, utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32_to_utf16u, utf32_char_t, src32, src32_e, 0, utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32_to_utf16x, utf32_char_t, src32, src32_e, 0, utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32_to_utf16ux, utf32_char_t, src32, src32_e, 0, utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32u_to_utf16, utf32_char_unaligned_t, src32u, src32u_e, 0, utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32u_to_utf16u, utf32_char_unaligned_t, src32u, src32u_e, 0, utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32u_to_utf16x, utf32_char_unaligned_t, src32u, src32u_e, 0, utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32u_to_utf16ux, utf32_char_unaligned_t, src32u, src32u_e, 0, utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32x_to_utf16, utf32_char_t, src32x, src32x_e, 0, utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32x_to_utf16u, utf32_char_t, src32x, src32x_e, 0, utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32x_to_utf16x, utf32_char_t, src32x, src32x_e, 0, utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32x_to_utf16ux, utf32_char_t, src32x, src32x_e, 0, utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32ux_to_utf16, utf32_char_unaligned_t, src32ux, src32ux_e, 0, utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32ux_to_utf16u, utf32_char_unaligned_t, src32ux, src32ux_e, 0, utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32ux_to_utf16x, utf32_char_unaligned_t, src32ux, src32ux_e, 0, utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(SZ, utf32ux_to_utf16ux, utf32_char_unaligned_t, src32ux, src32ux_e, 0, utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32_to_utf16_z, utf32_char_t, src32, src32_e, 1, utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32_to_utf16u_z, utf32_char_t, src32, src32_e, 1, utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32_to_utf16x_z, utf32_char_t, src32, src32_e, 1, utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32_to_utf16ux_z, utf32_char_t, src32, src32_e, 1, utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32u_to_utf16_z, utf32_char_unaligned_t, src32u, src32u_e, 1, utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32u_to_utf16u_z, utf32_char_unaligned_t, src32u, src32u_e, 1, utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32u_to_utf16x_z, utf32_char_unaligned_t, src32u, src32u_e, 1, utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32u_to_utf16ux_z, utf32_char_unaligned_t, src32u, src32u_e, 1, utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32x_to_utf16_z, utf32_char_t, src32x, src32x_e, 1, utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32x_to_utf16u_z, utf32_char_t, src32x, src32x_e, 1, utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32x_to_utf16x_z, utf32_char_t, src32x, src32x_e, 1, utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32x_to_utf16ux_z, utf32_char_t, src32x, src32x_e, 1, utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32ux_to_utf16_z, utf32_char_unaligned_t, src32ux, src32ux_e, 1, utf16_char_t, utf16_char_t, src16, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32ux_to_utf16u_z, utf32_char_unaligned_t, src32ux, src32ux_e, 1, utf16_char_t, utf16_char_unaligned_t, src16, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32ux_to_utf16x_z, utf32_char_unaligned_t, src32ux, src32ux_e, 1, utf16_char_t, utf16_char_t, src16x, utf16_buf, utf16_buf_sz, utf16_sz - 1/*BOM*/)
	TESTCONV(EMPTY, utf32ux_to_utf16ux_z, utf32_char_unaligned_t, src32ux, src32ux_e, 1, utf16_char_t, utf16_char_unaligned_t, src16x, ((utf16_char_unaligned_t*)utf16_buf), utf16_buf_sz, utf16_sz - 1/*BOM*/)
	return 0;
}

#include "test_data.inl"

int main(int argc, char *argv[])
{
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
		}
	}
	printf("All tests ok\n");
	(void)argc, (void)argv;
	return 0;
}
