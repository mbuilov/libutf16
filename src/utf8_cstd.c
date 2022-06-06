/**********************************************************************************
* C90/C99/C11 standard multibyte-to-widechar conversion routines for UTF-8 encoding
* Copyright (C) 2020-2022 Michael M. Builov, https://github.com/mbuilov/libutf16
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

#ifdef _MSC_VER
#pragma warning(disable:5045) /* Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified */
#pragma warning(disable:4711) /* function 'function' selected for inline expansion */
#endif

/* unsigned integer type must be at least of 32 bits */
typedef int check_unsigned_int_at_least_32_bits[1-2*((unsigned)-1 < 0xFFFFFFFF)];

int utf8_mblen(
	const utf8_char_t *const s,
	const size_t n)
{
	if (s) {
		utf8_state_t state = 0;
		const size_t count = utf8_len_one(s, n, &state);
		if (count < (size_t)-2)
			return (int)count; /* 0-4 */
		if ((size_t)-1 == count)
			errno = EILSEQ;
		return -1; /* incomplete/invalid unicode character */
	}
	return 0; /* UTF8 is a stateless encoding */
}

size_t utf8_mbrlen(
	const utf8_char_t *const LIBUTF16_RESTRICT s,
	const size_t n,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	static utf8_state_t mbrlen_state = 0;
	if (!ps)
		ps = &mbrlen_state;
	if (s) {
		const size_t count = utf8_len_one(s, n, ps);
		if ((size_t)-1 == count)
			errno = EILSEQ;
		return count; /* 0-4, (size_t)-2, (size_t)-1 */
	}
	if (!*ps)
		return 0;
	errno = EILSEQ;
	return (size_t)-1;
}

size_t utf8_mbrtoc16(
	utf16_char_t *const LIBUTF16_RESTRICT pwc,
	const utf8_char_t *const LIBUTF16_RESTRICT s,
	const size_t n,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	static utf8_state_t mbrtoc16_state = 0;
	if (!ps)
		ps = &mbrtoc16_state;
	if (s) {
		utf16_char_t x;
		const size_t count = utf8_to_utf16_one(pwc ? pwc : &x, s, n, ps);
		if ((size_t)-1 == count)
			errno = EILSEQ;
		return count; /* 0-4, (size_t)-3, (size_t)-2, (size_t)-1 */
	}
	if (!*ps)
		return 0;
	errno = EILSEQ;
	return (size_t)-1;
}

size_t utf8_mbrtoc32(
	utf32_char_t *const LIBUTF16_RESTRICT pwi,
	const utf8_char_t *const LIBUTF16_RESTRICT s,
	const size_t n,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	static utf8_state_t mbrtoc32_state = 0;
	if (!ps)
		ps = &mbrtoc32_state;
	if (s) {
		const size_t count = pwi ?
			utf8_to_utf32_one(pwi, s, n, ps) :
			utf8_len_one(s, n, ps);
		if ((size_t)-1 == count)
			errno = EILSEQ;
		return count; /* 0-4, (size_t)-2, (size_t)-1 */
	}
	if (!*ps)
		return 0;
	errno = EILSEQ;
	return (size_t)-1;
}

size_t utf8_c16rtomb(
	utf8_char_t *const LIBUTF16_RESTRICT s,
	const utf16_char_t wc,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	static utf8_state_t c16rtomb_state = 0;
	if (!ps)
		ps = &c16rtomb_state;
	if (s) {
		const size_t count = utf16_to_utf8_one(s, wc, ps);
		if ((size_t)-1 == count)
			errno = EILSEQ;
		return count; /* 0-4, (size_t)-1 */
	}
	if (!*ps)
		return 1;
	errno = EILSEQ;
	return (size_t)-1;
}

size_t utf8_c32rtomb(
	utf8_char_t *const LIBUTF16_RESTRICT s,
	const utf32_char_t wi,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	(void)ps;
	if (s) {
		const size_t count = utf32_to_utf8_one(s, wi);
		if ((size_t)-1 == count)
			errno = EILSEQ;
		return count; /* 1-4, (size_t)-1 */
	}
	return 1;
}

int utf8_mbtowc16_obsolete(
	utf16_char_t *const LIBUTF16_RESTRICT pwc,
	const utf8_char_t *const LIBUTF16_RESTRICT s,
	const size_t n)
{
	if (s) {
		utf8_state_t state = 0;
		const size_t count = pwc ?
			utf8_to_utf16_one(pwc, s, n, &state) :
			utf8_len_one(s, n, &state);
		if (count < 4)
			return (int)count; /* 0-3 */
		if (count == 4 ||
			/* A unicode character requires a utf16-surrogate
			   pair, which cannot be stored in a single utf16_char_t.
			   mbrtoc16/mbrtoc32 should be used in this case.  */
			(size_t)-1 == count)
		{
			errno = EILSEQ;
		}
		return -1; /* incomplete/invalid unicode character */
	}
	return 0; /* UTF8 is a stateless encoding.  */
}

int utf8_mbtowc32(
	utf32_char_t *const LIBUTF16_RESTRICT pwc,
	const utf8_char_t *const LIBUTF16_RESTRICT s,
	const size_t n)
{
	if (s) {
		utf8_state_t state = 0;
		const size_t count = pwc ?
			utf8_to_utf32_one(pwc, s, n, &state) :
			utf8_len_one(s, n, &state);
		if (count < (size_t)-2)
			return (int)count; /* 0-4 */
		if ((size_t)-1 == count)
			errno = EILSEQ;
		return -1; /* incomplete/invalid unicode character */
	}
	return 0; /* UTF8 is a stateless encoding.  */
}

size_t utf8_mbrtowc16_obsolete(
	utf16_char_t *const LIBUTF16_RESTRICT pwc,
	const utf8_char_t *const LIBUTF16_RESTRICT s,
	const size_t n,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	static utf8_state_t mbrtowc16_state = 0;
	if (!ps)
		ps = &mbrtowc16_state;
	if (s) {
		utf16_char_t x;
		const size_t count = utf8_to_utf16_one(pwc ? pwc : &x, s, n, ps);
		if (count < (size_t)-2 && *ps) {
			/* A unicode character requires a utf16-surrogate
			   pair, which cannot be stored in a single utf16_char_t.
			   mbrtoc16/mbrtoc32 should be used in this case.  */
			errno = EILSEQ;
			return (size_t)-1;
		}
		if ((size_t)-1 == count)
			errno = EILSEQ;
		return count; /* 0-3, (size_t)-2, (size_t)-1 */
	}
	if (!*ps)
		return 0;
	errno = EILSEQ;
	return (size_t)-1;
}

size_t utf8_mbrtowc32(
	utf32_char_t *const LIBUTF16_RESTRICT pwc,
	const utf8_char_t *const LIBUTF16_RESTRICT s,
	const size_t n,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	static utf8_state_t mbrtowc32_state = 0;
	return utf8_mbrtoc32(pwc, s, n, ps ? ps : &mbrtowc32_state);
}

int utf8_wc16tomb_obsolete(utf8_char_t s[3], const utf16_char_t wc)
{
	if (s) {
		utf8_state_t state = 0;
		const size_t count = utf16_to_utf8_one(s, wc, &state);
		if (!count) {
			/* Trying to convert a high part of utf16-surrogate pair.
			   But it's not possible to save a unicode character without
			   a low part of utf16-surrogate pair.
			   c16rtomb should be used in this case.  */
			return -1; /* wc can not be represented as a multibyte sequence */
		}
		if ((size_t)-1 == count)
			return -1; /* converting lower part of utf16-surrogate pair */
		return (int)count; /* 1-3 */
	}
	return 0; /* UTF8 is a stateless encoding.  */
}

int utf8_wc32tomb(utf8_char_t s[UTF8_MAX_LEN], const utf32_char_t wc)
{
	if (s) {
		const size_t count = utf32_to_utf8_one(s, wc);
		if ((size_t)-1 == count)
			return -1;
		return (int)count; /* 1-4 */
	}
	return 0; /* UTF8 is a stateless encoding.  */
}

size_t utf8_wc16rtomb_obsolete(
	utf8_char_t *const LIBUTF16_RESTRICT s,
	const utf16_char_t wc,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	(void)ps;
	if (s) {
		utf8_state_t state = 0;
		const size_t count = utf16_to_utf8_one(s, wc, &state);
		if (!count) {
			/* Trying to convert a high part of utf16-surrogate pair.
			   But it's not possible to save a unicode character without
			   a low part of utf16-surrogate pair.
			   c16rtomb should be used in this case.  */
			errno = EILSEQ;
			return (size_t)-1; /* wc can not be represented as a multibyte sequence */
		}
		if ((size_t)-1 == count)
			errno = EILSEQ; /* converting lower part of utf16-surrogate pair */
		return count; /* 1-3, (size_t)-1 */
	}
	return 1;
}

size_t utf8_wc32rtomb(
	utf8_char_t *const LIBUTF16_RESTRICT s,
	const utf32_char_t wi,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	return utf8_c32rtomb(s, wi, ps);
}

/* ================ mbstowcs()/wcstombs() ============== */

size_t utf8_mbstoc16s(
	utf16_char_t *const LIBUTF16_RESTRICT dst,
	const utf8_char_t *const LIBUTF16_RESTRICT src,
	size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		const utf8_char_t *q = src;
		utf16_char_t *b = dst;
		const size_t sz = utf8_to_utf16_z_(&q, &b, n, /*determ_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n)
			return sz - 1; /* don't count terminating nul */
		return (size_t)(b - dst);
	}
}

size_t utf8_c16stombs(
	utf8_char_t *const LIBUTF16_RESTRICT dst,
	const utf16_char_t *const LIBUTF16_RESTRICT src,
	size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		const utf16_char_t *w = src;
		utf8_char_t *b = dst;
		const size_t sz = utf16_to_utf8_z_(&w, &b, n, /*determ_size:*/!n);
		if (!sz) {
			if (w == src || w[-1])
				errno = EILSEQ;
			else
				errno = E2BIG; /* 3 utf8 bytes per 2 utf16 bytes overflows size_t */
			return (size_t)-1;
		}
		if (sz <= n || !n)
			return sz - 1; /* don't count terminating nul */
		return (size_t)(b - dst);
	}
}

size_t utf8_mbstoc32s(
	utf32_char_t *const LIBUTF16_RESTRICT dst,
	const utf8_char_t *const LIBUTF16_RESTRICT src,
	size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		const utf8_char_t *q = src;
		utf32_char_t *b = dst;
		const size_t sz = utf8_to_utf32_z_(&q, &b, n, /*determ_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n)
			return sz - 1; /* don't count terminating nul */
		return (size_t)(b - dst);
	}
}

size_t utf8_c32stombs(
	utf8_char_t *const LIBUTF16_RESTRICT dst,
	const utf32_char_t *const LIBUTF16_RESTRICT src,
	size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		const utf32_char_t *w = src;
		utf8_char_t *b = dst;
		const size_t sz = utf32_to_utf8_z_(&w, &b, n, /*determ_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n)
			return sz - 1; /* don't count terminating nul */
		return (size_t)(b - dst);
	}
}

size_t utf8_c16stoc32s(
	utf32_char_t *const LIBUTF16_RESTRICT dst,
	const utf16_char_t *const LIBUTF16_RESTRICT src,
	size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		const utf16_char_t *q = src;
		utf32_char_t *b = dst;
		const size_t sz = utf16_to_utf32_z_(&q, &b, n, /*determ_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n)
			return sz - 1; /* don't count terminating nul */
		return (size_t)(b - dst);
	}
}

size_t utf8_c32stoc16s(
	utf16_char_t *const LIBUTF16_RESTRICT dst,
	const utf32_char_t *const LIBUTF16_RESTRICT src,
	size_t n)
{
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		const utf32_char_t *w = src;
		utf16_char_t *b = dst;
		const size_t sz = utf32_to_utf16_z_(&w, &b, n, /*determ_size:*/!n);
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

size_t utf8_mbsrtoc16s(
	utf16_char_t *const LIBUTF16_RESTRICT dst,
	const utf8_char_t **const LIBUTF16_RESTRICT src,
	size_t n,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	(void)ps;
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		utf16_char_t *b = dst;
		const size_t sz = utf8_to_utf16_z_(src, &b, n, /*determ_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n) {
			if (n)
				*src = NULL;
			return sz - 1; /* don't count terminating nul */
		}
		return (size_t)(b - dst);
	}
}

size_t utf8_c16srtombs(
	utf8_char_t *const LIBUTF16_RESTRICT dst,
	const utf16_char_t **const LIBUTF16_RESTRICT src,
	size_t n,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	(void)ps;
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		utf8_char_t *b = dst;
		const utf16_char_t *const s = *src;
		const size_t sz = utf16_to_utf8_z_(src, &b, n, /*determ_size:*/!n);
		if (!sz) {
			if (s == *src || (*src)[-1])
				errno = EILSEQ;
			else
				errno = E2BIG; /* 3 utf8 bytes per 2 utf16 bytes overflows size_t */
			return (size_t)-1;
		}
		if (sz <= n || !n) {
			if (n)
				*src = NULL;
			return sz - 1; /* don't count terminating nul */
		}
		return (size_t)(b - dst);
	}
}

size_t utf8_mbsrtoc32s(
	utf32_char_t *const LIBUTF16_RESTRICT dst,
	const utf8_char_t **const LIBUTF16_RESTRICT src,
	size_t n,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	(void)ps;
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		utf32_char_t *b = dst;
		const size_t sz = utf8_to_utf32_z_(src, &b, n, /*determ_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n) {
			if (n)
				*src = NULL;
			return sz - 1; /* don't count terminating nul */
		}
		assert((size_t)(b - dst) == n);
	}
	return n;
}

size_t utf8_c32srtombs(
	utf8_char_t *const LIBUTF16_RESTRICT dst,
	const utf32_char_t **const LIBUTF16_RESTRICT src,
	size_t n,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	(void)ps;
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		utf8_char_t *b = dst;
		const size_t sz = utf32_to_utf8_z_(src, &b, n, /*determ_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n) {
			if (n)
				*src = NULL;
			return sz - 1; /* don't count terminating nul */
		}
		return (size_t)(b - dst);
	}
}

size_t utf8_c16srtoc32s(
	utf32_char_t *const LIBUTF16_RESTRICT dst,
	const utf16_char_t **const LIBUTF16_RESTRICT src,
	size_t n,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	(void)ps;
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		utf32_char_t *b = dst;
		const size_t sz = utf16_to_utf32_z_(src, &b, n, /*determ_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n) {
			if (n)
				*src = NULL;
			return sz - 1; /* don't count terminating nul */
		}
		assert((size_t)(b - dst) == n);
	}
	return n;
}

size_t utf8_c32srtoc16s(
	utf16_char_t *const LIBUTF16_RESTRICT dst,
	const utf32_char_t **const LIBUTF16_RESTRICT src,
	size_t n,
	utf8_state_t *LIBUTF16_RESTRICT ps)
{
	(void)ps;
	if (!dst)
		n = 0;
	else if (!n)
		return 0;
	{
		utf16_char_t *b = dst;
		const size_t sz = utf32_to_utf16_z_(src, &b, n, /*determ_size:*/!n);
		if (!sz) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (sz <= n || !n) {
			if (n)
				*src = NULL;
			return sz - 1; /* don't count terminating nul */
		}
		return (size_t)(b - dst);
	}
}
