/**********************************************************************************
* C90/C99/C11 standard multibyte-to-widechar conversion routines for UTF-8 encoding
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf8_cstd.c */

#include <errno.h>  /* for EILSEQ */
#include <stddef.h> /* for size_t */
#ifndef _WIN32
#include <stdint.h> /* for uint16_t */
#endif
#include "libutf16/utf8_cstd.h"
#include "libutf16/utf8_to_utf16.h"
#include "libutf16/utf16_to_utf8.h"
#include "libutf16/utf8_to_utf16_one.h"
#include "libutf16/utf16_to_utf8_one.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#define A_Restrict
#endif

#ifdef _MSC_VER
#pragma warning(disable:5045) /* Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified */
#pragma warning(disable:4711) /* function 'function' selected for inline expansion */
#endif

/* unsigned integer type must be at least of 32 bits */
typedef int check_unsigned_int_at_least_32_bits[1-2*((unsigned)-1 < 0xFFFFFFFF)];

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(3)
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout)
A_Success(return != -1)
#endif
static int utf8_mblen_(const utf8_char_t *const A_Restrict s, const size_t n,
	utf8_state_t *const A_Restrict ps)
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

A_Use_decl_annotations
int utf8_mblen(const utf8_char_t *const s, const size_t n)
{
	static utf8_state_t mblen_state = 0;
	return utf8_mblen_(s, n, &mblen_state);
}

A_Use_decl_annotations
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

A_Use_decl_annotations
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

A_Use_decl_annotations
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

A_Use_decl_annotations
size_t utf8_c16rtomb(utf8_char_t *const s, const utf16_char_t wc, utf8_state_t *ps)
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

A_Use_decl_annotations
size_t utf8_c32rtomb(utf8_char_t *const s, utf32_char_t wi)
{
	if (s) {
		const size_t count = utf32_to_utf8_one(s, wi);
		if ((size_t)-1 == count)
			errno = EILSEQ;
		return count; /* 1-4, (size_t)-1 */
	}
	return 1;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(5)
A_When(s, A_At(pwc, A_Out_opt))
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout)
A_Ret_range(-1, UTF8_MAX_LEN)
#endif
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

A_Use_decl_annotations
int utf8_mbtowc16(utf16_char_t *const pwc, const utf8_char_t *const s, const size_t n)
{
	static utf8_state_t mbtowc16_state = 0;
	return utf8_mbtowc_(/*is_wchar_16:*/1, pwc, s, n, &mbtowc16_state);
}

A_Use_decl_annotations
int utf8_mbtowc32(utf32_char_t *const pwc, const utf8_char_t *const s, const size_t n)
{
	static utf8_state_t mbtowc32_state = 0;
	return utf8_mbtowc_(/*is_wchar_16:*/0, pwc, s, n, &mbtowc32_state);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(5)
A_When(s, A_At(pwc, A_Out_opt))
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout)
A_Success(return != A_Size_t(-1))
#endif
static size_t utf8_mbrtowc_(const int is_wchar_16, void *const pwc,
	const utf8_char_t *const s, const size_t n, utf8_state_t *const ps)
{
	if (!pwc)
		return utf8_mbrlen(s, n, ps);
	if (s) {
		const size_t count = is_wchar_16
			? utf8_to_utf16_one((utf16_char_t*)pwc, s, n, ps)
			: utf8_to_utf32_one((utf32_char_t*)pwc, s, n, ps);
		if (count < (size_t)-2 && is_wchar_16 && *ps) {
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

A_Use_decl_annotations
size_t utf8_mbrtowc16(utf16_char_t *const pwc, const utf8_char_t *const s,
	const size_t n, utf8_state_t *const ps)
{
	static utf8_state_t mbrtowc16_state = 0;
	return utf8_mbrtowc_(/*is_wchar_16:*/1, pwc, s, n, ps ? ps : &mbrtowc16_state);
}

A_Use_decl_annotations
size_t utf8_mbrtowc32(utf32_char_t *const pwc, const utf8_char_t *const s,
	const size_t n, utf8_state_t *const ps)
{
	static utf8_state_t mbrtowc32_state = 0;
	return utf8_mbrtowc_(/*is_wchar_16:*/0, pwc, s, n, ps ? ps : &mbrtowc32_state);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_Out_writes_opt(UTF8_MAX_LEN))
A_At(wc16_state, A_Inout_opt)
A_Success(return != -1)
A_Ret_range(0, UTF8_MAX_LEN)
#endif
static int utf8_wctomb_(utf8_char_t *const s, const unsigned wc, utf8_state_t *const wc16_state)
{
	if (s) {
		const size_t count = wc16_state
			? utf16_to_utf8_one(s, (utf16_char_t)wc, wc16_state)
			: utf32_to_utf8_one(s, (utf32_char_t)wc);
		if (wc16_state && !count) {
			/* Trying to convert a high part of utf16-surrogate pair.
			   But it's not possible to save a unicode character without
			   a low part of utf16-surrogate pair.
			   c16rtomb should be in this case.  */
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

A_Use_decl_annotations
int utf8_wc16tomb(utf8_char_t *const s, const utf16_char_t wc)
{
	static utf8_state_t wc16tomb_state = 0;
	return utf8_wctomb_(s, wc, &wc16tomb_state);
}

A_Use_decl_annotations
int utf8_wc32tomb(utf8_char_t *const s, const utf32_char_t wc)
{
	return utf8_wctomb_(s, wc, /*wc16_state:*/NULL);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_Out_writes_opt(UTF8_MAX_LEN))
A_At(wc16_state, A_Inout_opt)
A_Success(return != A_Size_t(-1))
A_Ret_range(1, UTF8_MAX_LEN)
#endif
static size_t utf8_wcrtomb_(utf8_char_t *const s, const unsigned wc, utf8_state_t *const wc16_state)
{
	if (s) {
		const size_t count = wc16_state
			? utf16_to_utf8_one(s, (utf16_char_t)wc, wc16_state)
			: utf32_to_utf8_one(s, (utf32_char_t)wc);
		if (wc16_state && !count) {
			/* Trying to convert a high part of utf16-surrogate pair.
			   But it's not possible to save a unicode character without
			   a low part of utf16-surrogate pair.
			   c16rtomb should be in this case.  */
			errno = EILSEQ;
			return (size_t)-1; /* wc can not be represented as a multibyte sequence */
		}
		if ((size_t)-1 == count)
			errno = EILSEQ;
		return count; /* 1-4, (size_t)-1 */
	}
	if (!wc16_state || !*wc16_state)
		return 1;
	errno = EILSEQ;
	return (size_t)-1;
}

A_Use_decl_annotations
size_t utf8_wc16rtomb(utf8_char_t *const s, const utf16_char_t wc, utf8_state_t *ps)
{
	static utf8_state_t wc16rtomb_state = 0;
	return utf8_wcrtomb_(s, wc, ps ? ps : &wc16rtomb_state);
}

A_Use_decl_annotations
size_t utf8_wc32rtomb(utf8_char_t *const s, const utf32_char_t wc, utf8_state_t *ps)
{
	return utf8_wcrtomb_(s, wc, ps ? ps : /*wc16_state:*/NULL);
}

A_Use_decl_annotations
size_t utf8_mbstowc16s(utf16_char_t *const wcstr, const utf8_char_t *const mbstr, size_t count)
{
	if (!wcstr)
		count = 0;
	else if (!count)
		return 0;
	{
		const utf8_char_t *q = mbstr;
		utf16_char_t *b = wcstr;
		const size_t n = utf8_to_utf16_z(&q, &b, count);
		if (!n) {
			errno = EILSEQ;
			return (size_t)-1;
		}
		if (n <= count || !count)
			return n - 1; /* don't count terminating nul */
		return (size_t)(b - wcstr);
	}
}

A_Use_decl_annotations
size_t utf8_wc16stombs(utf8_char_t *const mbstr, const utf16_char_t *const wcstr, size_t count)
{
	if (!mbstr)
		count = 0;
	else if (!count)
		return 0;
	{
		const utf16_char_t *w = wcstr;
		utf8_char_t *b = mbstr;
		const size_t n = utf16_to_utf8_z(&w, &b, count);
		if (!n) {
			if (w == wcstr || w[-1])
				errno = EILSEQ;
			else
				/* 3 utf8 bytes per 2 utf16 bytes overflows size_t */
				errno = E2BIG;
			return (size_t)-1;
		}
		if (n <= count || !count)
			return n - 1; /* don't count terminating nul */
		return (size_t)(b - mbstr);
	}
}
