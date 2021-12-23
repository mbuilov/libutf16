/**********************************************************************************
* C90/C99/C11 standard multibyte-to-widechar conversion routines for UTF-8 encoding
* Copyright (C) 2020-2021 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf8_cstd.c */

#include <errno.h>  /* for EILSEQ */
#include <stddef.h> /* for size_t */
#ifndef _WIN32
#include <stdint.h> /* for uint16_t */
#endif
#include <assert.h>
#include "libutf16/utf8_cstd.h"
#include "libutf16/utf8_to_utf16.h"
#include "libutf16/utf16_to_utf8.h"
#include "libutf16/utf8_to_utf32.h"
#include "libutf16/utf32_to_utf8.h"
#include "libutf16/utf16_to_utf32.h"
#include "libutf16/utf32_to_utf16.h"
#include "libutf16/utf8_to_utf16_one.h"
#include "libutf16/utf16_to_utf8_one.h"

#ifndef FALLTHROUGH
#if defined __cplusplus && __cplusplus >= 201703L
#define FALLTHROUGH [[fallthrough]]
#elif defined __GNUC__ && __GNUC__ >= 7
#define FALLTHROUGH __attribute__ ((fallthrough))
#elif defined __cplusplus && defined __clang__ && __clang_major__ > 3 - (__clang_minor__ >= 6)
#define FALLTHROUGH [[clang::fallthrough]]
#else
#define FALLTHROUGH ((void)0)
#endif
#endif /* !FALLTHROUGH */

#ifdef _MSC_VER
#pragma warning(disable:5045) /* Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified */
#pragma warning(disable:4711) /* function 'function' selected for inline expansion */
#endif

/* unsigned integer type must be at least of 32 bits */
typedef int check_unsigned_int_at_least_32_bits[1-2*((unsigned)-1 < 0xFFFFFFFF)];

static int utf8_mblen_(const utf8_char_t *const s, const size_t n, utf8_state_t *const ps)
{
	if (s) {
		const size_t count = utf8_len_one(s, n, ps);
		if (count < (size_t)-2)
			/* return 0 for nul character */
			return (int)count; /* 0-4 */
		if ((size_t)-1 == count)
			errno = EILSEQ;
		else
			errno = 0; /* incomplete unicode character */
		return -1;
	}
	*ps = 0;
	return 0; /* UTF8 is a stateless encoding.  */
}

int utf8_mblen(const utf8_char_t *const s, const size_t n)
{
	static utf8_state_t mblen_state = 0;
	return utf8_mblen_(s, n, &mblen_state);
}

size_t utf8_mbrlen(const utf8_char_t *const s, const size_t n, utf8_state_t *ps)
{
	static utf8_state_t mbrlen_state = 0;
	if (!ps)
		ps = &mbrlen_state;
	if (s) {
		const size_t count = utf8_len_one(s, n, ps);
		if ((size_t)-1 == count)
			errno = EILSEQ;
		/* return 0 for nul character */
		return count; /* 0-4, (size_t)-2, (size_t)-1 */
	}
	if (!*ps)
		return 0;
	errno = EILSEQ;
	return (size_t)-1;
}

size_t utf8_mbrtoc16(utf16_char_t *const pwc, const utf8_char_t *const s,
	const size_t n, utf8_state_t *ps)
{
	static utf8_state_t mbrtoc16_state = 0;
	if (!ps)
		ps = &mbrtoc16_state;
	if (!pwc)
		return utf8_mbrlen(s, n, ps);
	if (s) {
		const size_t count = utf8_to_utf16_one(pwc, s, n, ps);
		if ((size_t)-1 == count)
			errno = EILSEQ;
		/* return 0 for nul character */
		return count; /* 0-4, (size_t)-3, (size_t)-2, (size_t)-1 */
	}
	if (!*ps)
		return 0;
	errno = EILSEQ;
	return (size_t)-1;
}

size_t utf8_mbrtoc32(utf32_char_t *const pwi, const utf8_char_t *const s,
	const size_t n, utf8_state_t *ps)
{
	static utf8_state_t mbrtoc32_state = 0;
	if (!ps)
		ps = &mbrtoc32_state;
	if (!pwi)
		return utf8_mbrlen(s, n, ps);
	if (s) {
		const size_t count = utf8_to_utf32_one(pwi, s, n, ps);
		if ((size_t)-1 == count)
			errno = EILSEQ;
		/* return 0 for nul character */
		return count; /* 0-4, (size_t)-2, (size_t)-1 */
	}
	if (!*ps)
		return 0;
	errno = EILSEQ;
	return (size_t)-1;
}

size_t utf8_c16rtomb(utf8_char_t s[UTF8_MAX_LEN], const utf16_char_t wc, utf8_state_t *ps)
{
	static utf8_state_t c16rtomb_state = 0;
	if (!ps)
		ps = &c16rtomb_state;
	if (s) {
		const size_t count = utf16_to_utf8_one(s, wc, ps);
		if ((size_t)-1 == count)
			errno = EILSEQ;
		/* return 0 if high utf16-surrogate has been saved in state */
		return count; /* 0-4, (size_t)-1 */
	}
	if (!*ps)
		return 1;
	errno = EILSEQ;
	return (size_t)-1;
}

size_t utf8_c32rtomb(utf8_char_t s[UTF8_MAX_LEN], const utf32_char_t wi, utf8_state_t *ps)
{
	if (s) {
		const size_t count = utf32_to_utf8_one(s, wi);
		if ((size_t)-1 == count)
			errno = EILSEQ;
		return count; /* 1-4, (size_t)-1 */
	}
	if (!ps || !*ps)
		return 1;
	errno = EILSEQ;
	return (size_t)-1;
}

static int utf8_mbtowc_(const int is_wchar_16, void *const pwc,
	const utf8_char_t *const s, const size_t n, utf8_state_t *const ps)
{
	if (!pwc)
		return utf8_mblen_(s, n, ps);
	if (s) {
		const size_t count = is_wchar_16
			? utf8_to_utf16_one((utf16_char_t*)pwc, s, n, ps)
			: utf8_to_utf32_one((utf32_char_t*)pwc, s, n, ps);
		if (count < (size_t)-2) {
			if (is_wchar_16 && *ps) {
				/* A unicode character requires a utf16-surrogate
				   pair, which cannot be stored in a single wchar_t.
				   mbrtoc16/mbrtoc32 should be used in this case.  */
				errno = EILSEQ;
				return -1;
			}
			/* return 0 for nul character */
			return (int)count; /* 0-4 */
		}
		if ((size_t)-1 == count)
			errno = EILSEQ;
		else
			errno = 0; /* incomplete unicode character */
		return -1;
	}
	*ps = 0;
	return 0; /* UTF8 is a stateless encoding.  */
}

int utf8_mbtowc16_obsolete(utf16_char_t *const pwc, const utf8_char_t *const s, const size_t n)
{
	static utf8_state_t mbtowc16_state = 0;
	return utf8_mbtowc_(/*is_wchar_16:*/1, pwc, s, n, &mbtowc16_state);
}

int utf8_mbtowc32(utf32_char_t *const pwc, const utf8_char_t *const s, const size_t n)
{
	static utf8_state_t mbtowc32_state = 0;
	return utf8_mbtowc_(/*is_wchar_16:*/0, pwc, s, n, &mbtowc32_state);
}

size_t utf8_mbrtowc16_obsolete(utf16_char_t *const pwc, const utf8_char_t *const s,
	const size_t n, utf8_state_t *ps)
{
	static utf8_state_t mbrtowc16_state = 0;
	if (!ps)
		ps = &mbrtowc16_state;
	if (!pwc)
		return utf8_mbrlen(s, n, ps);
	if (s) {
		const size_t count = utf8_to_utf16_one(pwc, s, n, ps);
		if (count < (size_t)-2 && *ps) {
			/* A unicode character requires a utf16-surrogate
			   pair, which cannot be stored in a single wchar_t.
			   mbrtoc16/mbrtoc32 should be used in this case.  */
			errno = EILSEQ;
			return (size_t)-1;
		}
		if ((size_t)-1 == count)
			errno = EILSEQ;
		/* return 0 for nul character */
		return count; /* 0-4, (size_t)-2, (size_t)-1 */
	}
	if (!*ps)
		return 0;
	errno = EILSEQ;
	return (size_t)-1;
}

size_t utf8_mbrtowc32(utf32_char_t *const pwc, const utf8_char_t *const s,
	const size_t n, utf8_state_t *ps)
{
	static utf8_state_t mbrtowc32_state = 0;
	return utf8_mbrtoc32(pwc, s, n, ps ? ps : &mbrtowc32_state);
}

static int utf8_wctomb_(utf8_char_t s[UTF8_MAX_LEN], const unsigned wc, utf8_state_t *const wc16_state)
{
	if (s) {
		const size_t count = wc16_state
			? utf16_to_utf8_one(s, (utf16_char_t)wc, wc16_state)
			: utf32_to_utf8_one(s, (utf32_char_t)wc);
		if (wc16_state && !count) {
			/* Trying to convert a high part of utf16-surrogate pair.
			   But it's not possible to save a unicode character without
			   a low part of utf16-surrogate pair.
			   c16rtomb should be used in this case.  */
			errno = EILSEQ;
			return -1; /* wc can not be represented as a multibyte sequence */
		}
		if ((size_t)-1 == count) {
			errno = EILSEQ;
			return -1;
		}
		return (int)count; /* 1-4 */
	}
	if (wc16_state)
		*wc16_state = 0;
	return 0; /* UTF8 is a stateless encoding.  */
}

int utf8_wc16tomb_obsolete(utf8_char_t s[UTF8_MAX_LEN], const utf16_char_t wc)
{
	static utf8_state_t wc16tomb_state = 0;
	return utf8_wctomb_(s, wc, &wc16tomb_state);
}

int utf8_wc32tomb(utf8_char_t s[UTF8_MAX_LEN], const utf32_char_t wc)
{
	return utf8_wctomb_(s, wc, /*wc16_state:*/NULL);
}

size_t utf8_wc16rtomb_obsolete(utf8_char_t s[UTF8_MAX_LEN], const utf16_char_t wc, utf8_state_t *ps)
{
	static utf8_state_t wc16rtomb_state = 0;
	if (!ps)
		ps = &wc16rtomb_state;
	if (s) {
		const size_t count = utf16_to_utf8_one(s, wc, ps);
		if (!count) {
			/* Trying to convert a high part of utf16-surrogate pair.
			   But it's not possible to save a unicode character without
			   a low part of utf16-surrogate pair.
			   c16rtomb should be used in this case.  */
			errno = EILSEQ;
			return (size_t)-1; /* wc can not be represented as a multibyte sequence */
		}
		if ((size_t)-1 == count)
			errno = EILSEQ;
		return count; /* 1-4, (size_t)-1 */
	}
	if (!*ps)
		return 1;
	errno = EILSEQ;
	return (size_t)-1;
}

/* ================ mbstowcs()/wcstombs() ============== */

size_t utf8_mbstoc16s(utf16_char_t dst[/*n*/], const utf8_char_t *const src, size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		const utf8_char_t *q = src;
		utf16_char_t *b = dst;
		const size_t sz = utf8_to_utf16_z_(&q, &b, n, /*determ_req_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n)
			return sz - 1; /* don't count terminating nul */
		return (size_t)(b - dst);
	}
}

size_t utf8_c16stombs(utf8_char_t dst[/*n*/], const utf16_char_t *const src, size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		const utf16_char_t *w = src;
		utf8_char_t *b = dst;
		const size_t sz = utf16_to_utf8_z_(&w, &b, n, /*determ_req_size:*/!n);
		if (!sz) {
			if (w == src || w[-1])
				errno = EILSEQ;
			else
				/* 3 utf8 bytes per 2 utf16 bytes overflows size_t */
				errno = E2BIG;
			return (size_t)-1;
		}
		if (sz <= n || !n)
			return sz - 1; /* don't count terminating nul */
		return (size_t)(b - dst);
	}
}

size_t utf8_mbstoc32s(utf32_char_t dst[/*n*/], const utf8_char_t *const src, size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		const utf8_char_t *q = src;
		utf32_char_t *b = dst;
		const size_t sz = utf8_to_utf32_z_(&q, &b, n, /*determ_req_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n)
			return sz - 1; /* don't count terminating nul */
		return (size_t)(b - dst);
	}
}

size_t utf8_c32stombs(utf8_char_t dst[/*n*/], const utf32_char_t *const src, size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		const utf32_char_t *w = src;
		utf8_char_t *b = dst;
		const size_t sz = utf32_to_utf8_z_(&w, &b, n, /*determ_req_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n)
			return sz - 1; /* don't count terminating nul */
		return (size_t)(b - dst);
	}
}

size_t utf8_c16stoc32s(utf32_char_t dst[/*n*/], const utf16_char_t *const src, size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		const utf16_char_t *q = src;
		utf32_char_t *b = dst;
		const size_t sz = utf16_to_utf32_z_(&q, &b, n, /*determ_req_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n)
			return sz - 1; /* don't count terminating nul */
		return (size_t)(b - dst);
	}
}

size_t utf8_c32stoc16s(utf16_char_t dst[/*n*/], const utf32_char_t *const src, size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		const utf32_char_t *w = src;
		utf16_char_t *b = dst;
		const size_t sz = utf32_to_utf16_z_(&w, &b, n, /*determ_req_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n)
			return sz - 1; /* don't count terminating nul */
		return (size_t)(b - dst);
	}
}

/* ================ mbsrtowcs()/wcsrtombs() ============== */

static utf16_char_t save_low_surrogate(const utf8_char_t **const src, utf8_state_t *ps)
{
	const utf8_char_t *const s = *src;
	unsigned a = s[0];
	a = (a << 6) + s[1];
	a = (a << 6) + s[2];
	a = (a << 6) + s[3] - 0x682080 - 0x10000;
	*src = s + 4;
	*ps = (a & 0x3FF) + 0xDC00;     /* low surrogate:  110111bbcccccccc */
	return (utf16_char_t)(a >> 10); /* high surrogate: 110110aaaabbbbbb */
}

size_t utf8_mbsrtoc16s(utf16_char_t dst[/*n*/], const utf8_char_t **const src, size_t n, utf8_state_t *ps)
{
	static utf8_state_t utf8_mbsrtoc16s_state = 0;
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	if (!ps)
		ps = &utf8_mbsrtoc16s_state;
	{
		utf16_char_t *b = dst;
		const utf16_char_t saved = (utf16_char_t)*ps;
		if (saved) {
			*ps = 0;
			if (b) {
				*b++ = saved;
				if (!--n)
					return 1;
			}
		}
		{
			const size_t sz = utf8_to_utf16_z_(src, &b, n, /*determ_req_size:*/!n);
			if (!sz) {
				errno = EILSEQ;
				return (size_t)-1;
			}
			if (sz <= n || !n) {
				*src = NULL;
				return sz - 1 + !!saved; /* don't count terminating nul */
			}
		}
		n += !!saved;
		if ((size_t)(b - dst) != n) {
			/* there is a room for high-surrogate of utf16 surrogate pair */
			assert(b + 1 == &dst[n]);
			*b = save_low_surrogate(src, ps/*out*/);
		}
	}
	return n;
}

static size_t read_saved_utf8(const unsigned saved,
	unsigned *const ps, utf8_char_t **const b, const size_t n)
{
	const unsigned x = saved >> 30;
	unsigned c = saved;
	assert(x); /* 1,2,3 */
	if (x == 2)
		goto x2;
	if (x != 3)
		goto x1;
	*(*b)++ = (utf8_char_t)((c >> 12) & 0xFF);
	c = (c & 0xFFF) + 0x2000 + 0x80000000;
	if (n == 1)
		goto ret;
x2:
	*(*b)++ = (utf8_char_t)((c >> 6) & 0xFF);
	c = (c & 0x3F) + 0x80 + 0x40000000;
	if (n < x) /* (x == 3 && n == 2) || (x == 2 && n == 1) */
		goto ret;
x1:
	*(*b)++ = (utf8_char_t)(c & 0xFF);
	c = 0;
	if (n > x)
		return 0;
ret:
	*ps = c;
	return n;
}

static unsigned save_part_utf16(utf8_char_t *b,
	const utf8_char_t *const lim, const utf16_char_t **const src)
{
	const utf16_char_t *w = *src;
	unsigned c = *w++;
	assert(c >= 0x80);
	if (c >= 0x800) {
		if (0xD800 == (c & 0xFC00)) {
			c = (c << 10) + (unsigned)*w++ - 0x20DC00 + 0x800000 + 0x10000;
			*b = (utf8_char_t)(c >> 18);
			c = (c & 0x3FFFF) + 0x80000;
			if (++b == lim) {
				c += 0xC0000000;
				goto ret;
			}
		}
		else
			c += 0xE0000;
		*b = (utf8_char_t)(c >> 12);
		c = (c & 0xFFF) + 0x2000;
		if (++b == lim) {
			c += 0x80000000;
			goto ret;
		}
	}
	else
		c += 0x3000;
	*b = (utf8_char_t)(c >> 6);
	c = (c & 0x3F) + 0x80;
	assert(b + 1 == lim);
	c += 0x40000000;
ret:
	*src = w;
	return c;
}

static unsigned save_part_utf32(utf8_char_t *b,
	const utf8_char_t *const lim, const utf32_char_t **const src)
{
	const utf32_char_t *w = *src;
	unsigned c = *w++;
	assert(c >= 0x80);
	if (c >= 0x800) {
		if (c > 0xFFFF) {
			c += 0x3C00000;
			*b = (utf8_char_t)(c >> 18);
			c = (c & 0x3FFFF) + 0x80000;
			if (++b == lim) {
				c += 0xC0000000;
				goto ret;
			}
		}
		else
			c += 0xE0000;
		*b = (utf8_char_t)(c >> 12);
		c = (c & 0xFFF) + 0x2000;
		if (++b == lim) {
			c += 0x80000000;
			goto ret;
		}
	}
	else
		c += 0x3000;
	*b = (utf8_char_t)(c >> 6);
	c = (c & 0x3F) + 0x80;
	assert(b + 1 == lim);
	c += 0x40000000;
ret:
	*src = w;
	return c;
}

size_t utf8_c16srtombs(utf8_char_t dst[/*n*/], const utf16_char_t **const src, size_t n, utf8_state_t *ps)
{
	static utf8_state_t utf8_c16srtombs_state = 0;
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	if (!ps)
		ps = &utf8_c16srtombs_state;
	{
		utf8_char_t *b = dst;
		const unsigned saved = *ps;
		if (saved) {
			if (b) {
				const size_t t = read_saved_utf8(saved, ps, &b, n);
				if (t)
					return t;
			}
			*ps = 0;
		}
		{
			const utf16_char_t *const s = *src;
			const size_t sz = utf16_to_utf8_z_(src, &b, n, /*determ_req_size:*/!n);
			if (!sz) {
				if (s == *src || (*src)[-1])
					errno = EILSEQ;
				else
					/* 3 utf8 bytes per 2 utf16 bytes overflows size_t */
					errno = E2BIG;
				return (size_t)-1;
			}
			if (sz <= n || !n) {
				*src = NULL;
				return sz - 1 + (saved >> 30); /* don't count terminating nul */
			}
		}
		n += saved >> 30;
		if ((size_t)(b - dst) != n) {
			/* there is a room for at least one byte of utf8-encoded non-one-byte unicode character */
			*ps = save_part_utf16(b, &dst[n], src);
		}
	}
	return n;
}

size_t utf8_mbsrtoc32s(utf32_char_t dst[/*n*/], const utf8_char_t **const src, size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		utf32_char_t *b = dst;
		const size_t sz = utf8_to_utf32_z_(src, &b, n, /*determ_req_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n) {
			*src = NULL;
			return sz - 1; /* don't count terminating nul */
		}
		assert((size_t)(b - dst) == n);
	}
	return n;
}

size_t utf8_c32srtombs(utf8_char_t dst[/*n*/], const utf32_char_t **const src, size_t n, utf8_state_t *ps)
{
	static utf8_state_t utf8_c32srtombs_state = 0;
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	if (!ps)
		ps = &utf8_c32srtombs_state;
	{
		utf8_char_t *b = dst;
		const unsigned saved = *ps;
		if (saved) {
			if (b) {
				const size_t t = read_saved_utf8(saved, ps, &b, n);
				if (t)
					return t;
			}
			*ps = 0;
		}
		{
			const size_t sz = utf32_to_utf8_z_(src, &b, n, /*determ_req_size:*/!n);
			if (!sz) {
				errno = EILSEQ;
				return (size_t)-1;
			}
			if (sz <= n || !n) {
				*src = NULL;
				return sz - 1 + (saved >> 30); /* don't count terminating nul */
			}
		}
		n += saved >> 30;
		if ((size_t)(b - dst) != n) {
			/* there is a room for at least one byte of utf8-encoded non-one-byte unicode character */
			*ps = save_part_utf32(b, &dst[n], src);
		}
	}
	return n;
}

size_t utf8_c16srtoc32s(utf32_char_t dst[/*n*/], const utf16_char_t **const src, size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		utf32_char_t *b = dst;
		const size_t sz = utf16_to_utf32_z_(src, &b, n, /*determ_req_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n) {
			*src = NULL;
			return sz - 1; /* don't count terminating nul */
		}
		assert((size_t)(b - dst) == n);
	}
	return n;
}

size_t utf8_c32srtoc16s(utf16_char_t dst[/*n*/], const utf32_char_t **const src, size_t n, utf8_state_t *ps)
{
	static utf8_state_t utf8_c32srtoc16s_state = 0;
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	if (!ps)
		ps = &utf8_c32srtoc16s_state;
	{
		utf16_char_t *b = dst;
		const utf16_char_t saved = (utf16_char_t)*ps;
		if (saved) {
			*ps = 0;
			if (b) {
				*b++ = saved;
				if (!--n)
					return 1;
			}
		}
		{
			const size_t sz = utf32_to_utf16_z_(src, &b, n, /*determ_req_size:*/!n);
			if (!sz) {
				errno = EILSEQ;
				return (size_t)-1;
			}
			if (sz <= n || !n) {
				*src = NULL;
				return sz - 1 + !!saved; /* don't count terminating nul */
			}
		}
		n += !!saved;
		if ((size_t)(b - dst) != n) {
			/* there is a room for high-surrogate of utf16 surrogate pair */
			const unsigned c = *(*src)++;
			assert(0xFFFF < c && c <= 0x10FFFF);
			*b = utf32_get_high_surrogate(c);
			*ps = utf32_get_low_surrogate(c);
			assert(b + 1 == &dst[n]);
		}
	}
	return n;
}

/* ================ non-standard extension ============== */

size_t utf8_mbsrtoc16s_nz(utf16_char_t dst[/*n*/], const utf8_char_t **const src,
	size_t nsrc, size_t n, utf8_state_t *ps/*!=NULL*/)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		utf16_char_t *b = dst;
		const utf16_char_t saved = (utf16_char_t)*ps;
		if (saved) {
			*ps = 0;
			if (b) {
				*b++ = saved;
				if (!--n) {
					if (!nsrc)
						*src = NULL;
					return 1;
				}
			}
		}
		if (!nsrc) {
			*src = NULL;
			return !!saved;
		}
		{
			const size_t sz = utf8_to_utf16_(src, &b, n, nsrc/*>0*/, /*determ_req_size:*/!n);
			if (!sz) {
				errno = EILSEQ;
				return (size_t)-1;
			}
			if (sz <= n || !n) {
				*src = NULL;
				return sz + !!saved;
			}
		}
		n += !!saved;
		if ((size_t)(b - dst) != n) {
			/* there is a room for high-surrogate of utf16 surrogate pair */
			assert(b + 1 == &dst[n]);
			*b = save_low_surrogate(src, ps/*out*/);
		}
	}
	return n;
}

size_t utf8_c16srtombs_nz(utf8_char_t dst[/*n*/], const utf16_char_t **const src,
	size_t nsrc, size_t n, utf8_state_t *ps/*!=NULL*/)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		utf8_char_t *b = dst;
		const unsigned saved = *ps;
		if (saved) {
			if (b) {
				const size_t t = read_saved_utf8(saved, ps, &b, n);
				if (t) {
					if (!nsrc && !*ps)
						*src = NULL;
					return t;
				}
			}
			*ps = 0;
		}
		if (!nsrc) {
			*src = NULL;
			return saved >> 30;
		}
		{
			const utf16_char_t *const s = *src;
			const size_t sz = utf16_to_utf8_(src, &b, n, nsrc/*>0*/, /*determ_req_size:*/!n);
			if (!sz) {
				if (s + nsrc != *src)
					errno = EILSEQ;
				else
					/* 3 utf8 bytes per 2 utf16 bytes overflows size_t */
					errno = E2BIG;
				return (size_t)-1;
			}
			if (sz <= n || !n) {
				*src = NULL;
				return sz + (saved >> 30);
			}
		}
		n += saved >> 30;
		if ((size_t)(b - dst) != n) {
			/* there is a room for at least one byte of utf8-encoded non-one-byte unicode character */
			*ps = save_part_utf16(b, &dst[n], src);
		}
	}
	return n;
}

size_t utf8_mbsrtoc32s_nz(utf32_char_t dst[/*n*/], const utf8_char_t **const src,
	size_t nsrc, size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	if (!nsrc) {
		*src = NULL;
		return 0;
	}
	{
		utf32_char_t *b = dst;
		const size_t sz = utf8_to_utf32_(src, &b, n, nsrc/*>0*/, /*determ_req_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n) {
			*src = NULL;
			return sz;
		}
		assert((size_t)(b - dst) == n);
	}
	return n;
}

size_t utf8_c32srtombs_nz(utf8_char_t dst[/*n*/], const utf32_char_t **const src,
	size_t nsrc, size_t n, utf8_state_t *ps/*!=NULL*/)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		utf8_char_t *b = dst;
		const unsigned saved = *ps;
		if (saved) {
			if (b) {
				const size_t t = read_saved_utf8(saved, ps, &b, n);
				if (t) {
					if (!nsrc && !*ps)
						*src = NULL;
					return t;
				}
			}
			*ps = 0;
		}
		if (!nsrc) {
			*src = NULL;
			return saved >> 30;
		}
		{
			const size_t sz = utf32_to_utf8_(src, &b, n, nsrc/*>0*/, /*determ_req_size:*/!n);
			if (!sz) {
				errno = EILSEQ;
				return (size_t)-1;
			}
			if (sz <= n || !n) {
				*src = NULL;
				return sz + (saved >> 30);
			}
		}
		n += saved >> 30;
		if ((size_t)(b - dst) != n) {
			/* there is a room for at least one byte of utf8-encoded non-one-byte unicode character */
			*ps = save_part_utf32(b, &dst[n], src);
		}
	}
	return n;
}

size_t utf8_c16srtoc32s_nz(utf32_char_t dst[/*n*/], const utf16_char_t **const src,
	size_t nsrc, size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	if (!nsrc) {
		*src = NULL;
		return 0;
	}
	{
		utf32_char_t *b = dst;
		const size_t sz = utf16_to_utf32_(src, &b, n, nsrc/*>0*/, /*determ_req_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n) {
			*src = NULL;
			return sz;
		}
		assert((size_t)(b - dst) == n);
	}
	return n;
}

size_t utf8_c32srtoc16s_nz(utf16_char_t dst[/*n*/], const utf32_char_t **const src,
	size_t nsrc, size_t n, utf8_state_t *ps/*!=NULL*/)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		utf16_char_t *b = dst;
		const utf16_char_t saved = (utf16_char_t)*ps;
		if (saved) {
			*ps = 0;
			if (b) {
				*b++ = saved;
				if (!--n) {
					if (!nsrc)
						*src = NULL;
					return 1;
				}
			}
		}
		if (!nsrc) {
			*src = NULL;
			return !!saved;
		}
		{
			const size_t sz = utf32_to_utf16_(src, &b, n, nsrc/*>0*/, /*determ_req_size:*/!n);
			if (!sz) {
				errno = EILSEQ;
				return (size_t)-1;
			}
			if (sz <= n || !n) {
				*src = NULL;
				return sz + !!saved;
			}
		}
		n += !!saved;
		if ((size_t)(b - dst) != n) {
			/* there is a room for high-surrogate of utf16 surrogate pair */
			const unsigned c = *(*src)++;
			assert(0xFFFF < c && c <= 0x10FFFF);
			*b = utf32_get_high_surrogate(c);
			*ps = utf32_get_low_surrogate(c);
			assert(b + 1 == &dst[n]);
		}
	}
	return n;
}
