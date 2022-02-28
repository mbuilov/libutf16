#ifndef UTF8_CSTD_H_INCLUDED
#define UTF8_CSTD_H_INCLUDED

/**********************************************************************************
* C90/C99/C11 standard multibyte-to-widechar conversion routines for UTF-8 encoding
* Copyright (C) 2020-2022 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf8_cstd.h */

#include "utf16_char.h"

#ifdef __cplusplus
extern "C" {
#endif

/* mblen (3) */
/* returns:
 s != NULL:
  -1   on incomplete character (or n is zero) (do not sets errno) or on error (errno == EILSEQ)
   0   for nul character (1 byte read)
   1-4 bytes read
 s == NULL:
   0   UTF8 is a stateless encoding */
/* note: no internal state is preserved between calls */
int utf8_mblen(
	const utf8_char_t *const s/*[n],NULL?*/,
	const size_t n/*>=0*/);

/* mbrlen (3) */
/* returns:
 s != NULL:
  -1   on error (errno == EILSEQ, state not changed)
  -2   on incomplete character (or n is zero) (n bytes consumed, state updated if n is non-zero)
   0   for nul character (1 byte read, state is zero)
   1-4 bytes read (state is zero)
 s == NULL:
  -1   if state is non-zero (errno == EILSEQ, state not changed)
   0   if state is zero */
/* note: standard does not specify function behaviour for s == NULL,
  this version follows mbrtowc() convention: processes NULL as an empty string */
/* note: if ps is NULL, a private state is used; there is no way to reset it on error */
size_t utf8_mbrlen(
	const utf8_char_t *const LIBUTF16_RESTRICT s/*[n],NULL?*/,
	const size_t n/*>=0*/,
	utf8_state_t *LIBUTF16_RESTRICT ps/*NULL?*/);

/* mbrtoc16 (3) */
/* returns:
 s != NULL:
  -1   on error (errno == EILSEQ, state not changed)
  -2   on incomplete character (or n is zero and state do not contains a second part
       of utf16 surrogate pair) (n bytes consumed, state updated if n is non-zero)
  -3   second part of utf16 surrogate pair has been read from state
       (no source bytes consumed - n may be zero, state was reset to zero)
   0   for nul character (1 byte read, state is zero)
   1-4 bytes read (state may contain second part of utf16 surrogate pair)
 s == NULL:
  -1   if state is non-zero (errno == EILSEQ, state not changed)
   0   if state is zero */
/* note: if ps is NULL, a private state is used; there is no way to reset it on error */
size_t utf8_mbrtoc16(
	utf16_char_t *const LIBUTF16_RESTRICT pwc/*NULL?*/,
	const utf8_char_t *const LIBUTF16_RESTRICT s/*[n],NULL?*/,
	const size_t n/*>=0*/,
	utf8_state_t *LIBUTF16_RESTRICT ps/*NULL?*/);

/* mbrtoc32 (3) */
/* returns:
 s != NULL:
  -1   on error (errno == EILSEQ, state not changed)
  -2   on incomplete character (or n is zero) (n bytes consumed, state updated if n is non-zero)
   0   for nul character (1 byte read, state is zero)
   1-4 bytes read (state is zero)
 s == NULL:
  -1   if state is non-zero (errno == EILSEQ, state not changed)
   0   if state is zero */
/* note: if ps is NULL, a private state is used; there is no way to reset it on error */
size_t utf8_mbrtoc32(
	utf32_char_t *const LIBUTF16_RESTRICT pwi/*NULL?*/,
	const utf8_char_t *const LIBUTF16_RESTRICT s/*[n],NULL?*/,
	const size_t n/*>=0*/,
	utf8_state_t *LIBUTF16_RESTRICT ps/*NULL?*/);

/* c16rtomb (3) */
/* returns:
 s != NULL:
  -1   on error (errno == EILSEQ, state not changed)
   0   no bytes written, first part of utf16 surrogate pair has been saved in state
   1-4 bytes written (state is zero)
 s == NULL:
  -1   if state is non-zero (errno == EILSEQ, state not changed)
   1   if state is zero: one byte is needed to encode nul character */
/* note: if ps is NULL, a private state is used; there is no way to reset it on error */
size_t utf8_c16rtomb(
	utf8_char_t *const LIBUTF16_RESTRICT s/*[UTF8_MAX_LEN],NULL?*/,
	const utf16_char_t wc,
	utf8_state_t *LIBUTF16_RESTRICT ps/*NULL?*/);

/* c32rtomb (3) */
/* returns:
 s != NULL:
  -1   on error (errno == EILSEQ)
   1-4 bytes written
 s == NULL:
   1   one byte is needed to encode nul character */
/* note: this function do not uses state, *ps must be zero (if ps is not NULL) */
size_t utf8_c32rtomb(
	utf8_char_t s[UTF8_MAX_LEN]/*NULL?*/,
	const utf32_char_t wi,
	utf8_state_t *ps/*NULL?,not used*/);

/* mbtowc (3) for 16 bit wchar_t */
/* returns:
 s != NULL:
  -1   on incomplete character (or n is zero) (do not sets errno) or on error (errno == EILSEQ)
   0   for nul character (1 byte read)
   1-3 bytes read
 s == NULL:
   0   UTF8 is a stateless encoding */
/* note: this function do not handles utf16 surrogate pairs! Use utf8_mbrtoc16 instead! */
/* note: if unicode character do not fits utf16_char_t, -1 is returned and errno is set to EILSEQ
  indicating a decoding error, but (*pwc), if pwc is not NULL, will contain a high surrogate of
  the unicode character (this is the only case when this function may store a surrogate in *pwc);
  also, utf8_mblen() will return UTF8_MAX_LEN for the array s[n] */
int utf8_mbtowc16_obsolete(
	utf16_char_t *const LIBUTF16_RESTRICT pwc/*NULL?*/,
	const utf8_char_t *const LIBUTF16_RESTRICT s/*[n],NULL?*/,
	const size_t n/*>=0*/);

/* mbtowc (3) for 32 bit wchar_t */
/* returns:
 s != NULL:
  -1   on incomplete character (or n is zero) (do not sets errno) or on error (errno == EILSEQ)
   0   for nul character (1 byte read)
   1-4 bytes read
 s == NULL:
   0   UTF8 is a stateless encoding */
int utf8_mbtowc32(
	utf32_char_t *const LIBUTF16_RESTRICT pwc/*NULL?*/,
	const utf8_char_t *const LIBUTF16_RESTRICT s/*[n],NULL?*/,
	const size_t n/*>=0*/);

/* mbrtowc (3) for 16 bit wchar_t */
/* returns:
 s != NULL:
  -1   on error (errno == EILSEQ, state may be updated only if a unicode character do not fits
       utf16_char_t - see notes, else state not changed)
  -2   on incomplete character (or n is zero) (n bytes consumed, state updated if n is non-zero)
   0   for nul character (1 byte read, state is zero)
   1-3 bytes read (state is zero)
 s == NULL:
  -1   if state is non-zero (errno == EILSEQ, state not changed)
   0   if state is zero */
/* note: this function do not handles utf16 surrogate pairs! Use utf8_mbrtoc16 instead! */
/* note: if ps is NULL, a private state is used; there is no way to reset it on error */
/* note: if unicode character do not fits utf16_char_t, -1 is returned and errno is set to EILSEQ
  indicating a decoding error, but (*pwc), if pwc is not NULL, will contain a high surrogate of
  the unicode character (this is the only case when this function may store a surrogate in *pwc)
  and the state will contain a low surrogate of the unicode character (also, this is the only
  case when the state may contain a surrogate) */
size_t utf8_mbrtowc16_obsolete(
	utf16_char_t *const LIBUTF16_RESTRICT pwc/*NULL?*/,
	const utf8_char_t *const LIBUTF16_RESTRICT s/*[n],NULL?*/,
	const size_t n/*>=0*/,
	utf8_state_t *LIBUTF16_RESTRICT ps/*NULL?*/);

/* mbrtowc (3) for 32 bit wchar_t */
/* returns:
 s != NULL:
  -1   on error (errno == EILSEQ, state not changed)
  -2   on incomplete character (or n is zero) (n bytes consumed, state updated if n is non-zero)
   0   for nul character (1 byte read, state is zero)
   1-4 bytes read (state is zero)
 s == NULL:
  -1   if state is non-zero (errno == EILSEQ, state not changed)
   0   if state is zero */
/* note: if ps is NULL, a private state is used; there is no way to reset it on error */
size_t utf8_mbrtowc32(
	utf32_char_t *const LIBUTF16_RESTRICT pwc/*NULL?*/,
	const utf8_char_t *const LIBUTF16_RESTRICT s/*[n],NULL?*/,
	const size_t n/*>=0*/,
	utf8_state_t *LIBUTF16_RESTRICT ps/*NULL?*/);

/* wctomb (3) for 16 bit wchar_t */
/* returns:
 s != NULL:
  -1   on error (errno is not set) (wc is a surrogate)
   1-3 bytes written
 s == NULL:
   0   UTF8 is a stateless encoding */
/* note: this function do not handles utf16 surrogate pairs! Use utf8_c16rtomb instead! */
int utf8_wc16tomb_obsolete(
	utf8_char_t s[3]/*NULL?*/,
	const utf16_char_t wc);

/* wctomb (3) for 32 bit wchar_t */
/* returns:
 s != NULL:
  -1   on error (errno is not set)
   1-4 bytes written
 s == NULL:
   0   UTF8 is a stateless encoding */
int utf8_wc32tomb(
	utf8_char_t s[UTF8_MAX_LEN]/*NULL?*/,
	const utf32_char_t wc);

/* wcrtomb (3) for 16 bit wchar_t */
/* returns:
 s != NULL:
  -1   on error (errno == EILSEQ) (wc is a surrogate)
   1-3 bytes written
 s == NULL:
   1   one byte is needed to encode nul character */
/* note: this function do not handles utf16 surrogate pairs! Use utf8_c16rtomb instead! */
/* note: this function do not uses state, *ps must be zero (if ps is not NULL) */
size_t utf8_wc16rtomb_obsolete(
	utf8_char_t s[3]/*NULL?*/,
	const utf16_char_t wc,
	utf8_state_t *ps/*NULL?,not used*/);

/* wcrtomb (3) for 32 bit wchar_t */
/* returns:
 s != NULL:
  -1   on error (errno == EILSEQ)
   1-4 bytes written
 s == NULL:
   1   one byte is needed to encode nul character */
/* note: this function do not uses state, *ps must be zero (if ps is not NULL) */
#define utf8_wc32rtomb utf8_c32rtomb

/* ================ mbstowcs()/wcstombs() ============== */

/* mbstowcs (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 dst != NULL:
   number of utf16 characters stored, not counting terminating nul, which is not stored if buffer is too small.
   Note: return value < n do not guarantees that output was not truncated and terminating nul was stored, due
   to a surrogate pair - to be sure that terminating nul was written, caller should fill "check" zone at tail
   of output buffer of at least 1 non-zero utf16 character bytes - if buffer is large enough, nul character
   will be written over after the last filled character, possibly to "check" zone.
 dst == NULL:
   size of buffer (in utf16-chars) required to store all utf16 characters, not counting terminating nul */
size_t utf8_mbstoc16s(
	utf16_char_t *const LIBUTF16_RESTRICT dst/*[n],NULL?*/,
	const utf8_char_t *const LIBUTF16_RESTRICT src/*!=NULL,nul-terminated*/,
	size_t n/*>=0*/);

/* mbstowcs (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 dst != NULL:
   number of utf32 characters stored, not counting terminating nul, which is not written if buffer is too small.
 dst == NULL:
   size of buffer (in utf32-chars) required to store all utf32 characters, not counting terminating nul */
size_t utf8_mbstoc32s(
	utf32_char_t *const LIBUTF16_RESTRICT dst/*[n],NULL?*/,
	const utf8_char_t *const LIBUTF16_RESTRICT src/*!=NULL,nul-terminated*/,
	size_t n/*>=0*/);

/* wcstombs (3) */
/* returns:
  -1   on error (errno == EILSEQ or errno == E2BIG if utf16 string is too long)
 dst != NULL:
   number of utf8 bytes stored, not counting terminating nul, which is not written if buffer is too small.
   Tip: if caller wants to check if terminating nul was written, caller should fill "check" zone at tail of
   output buffer of at least (UTF8_MAX_LEN - 1) non-zero utf8 bytes - if buffer is large enough, nul character
   will be written after the last filled character, possibly to "check" zone.
 dst == NULL:
   size of buffer (in bytes) required to store all utf8 bytes, not counting terminating nul */
size_t utf8_c16stombs(
	utf8_char_t dst[/*n*/],
	const utf16_char_t *const src,
	size_t n);

/* wcstombs (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 dst != NULL:
   number of utf8 bytes stored, not counting terminating nul, which is not written if buffer is too small.
   Tip: if caller wants to check if terminating nul was written, caller should fill "check" zone at tail of
   output buffer of at least (UTF8_MAX_LEN - 1) non-zero utf8 bytes - if buffer is large enough, nul character
   will be written after the last filled character, possibly to "check" zone.
 dst == NULL:
   size of buffer (in bytes) required to store all utf8 bytes, not counting terminating nul */
size_t utf8_c32stombs(
	utf8_char_t dst[/*n*/],
	const utf32_char_t *const src,
	size_t n);

/* mbstowcs (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 dst != NULL:
   number of utf32 characters stored, not counting terminating nul, which is not written if buffer is too small.
 dst == NULL:
   size of buffer (in utf32-chars) required to store all utf32 characters, not counting terminating nul */
size_t utf8_c16stoc32s(
	utf32_char_t dst[/*n*/],
	const utf16_char_t *const src,
	size_t n);

/* wcstombs (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 dst != NULL:
   number of utf16 characters stored, not counting terminating nul, which is not written if buffer is too small.
   Tip: if caller wants to check if terminating nul was written, caller should fill "check" zone at tail of
   output buffer of at least 1 non-zero utf16 character bytes - if buffer is large enough, nul character
   will be written after the last filled character, possibly to "check" zone.
 dst == NULL:
   size of buffer (in utf16-chars) required to store all utf16 characters, not counting terminating nul */
size_t utf8_c32stoc16s(
	utf16_char_t dst[/*n*/],
	const utf32_char_t *const src,
	size_t n);

/* ================ mbsrtowcs()/wcsrtombs() ============== */

/* mbsrtowcs (3) */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf8 multibyte sequence,
 dst != NULL:
   number of utf16 characters stored, not counting terminating nul:
   a) if terminating nul was stored, *src will be set to NULL, *ps - to initial state,
   b) else (output buffer is too small), *src will point to the next utf8 multibyte sequence to process.
 dst == NULL:
   size of buffer (in utf16-chars) required to store all utf16 characters, not counting terminating nul */
size_t utf8_mbsrtoc16s(
	utf16_char_t dst[/*n*/],
	const utf8_char_t **const src,
	size_t n,
	utf8_state_t *ps);

/* wcsrtombs (3) */
/* returns:
  -1   on error (errno == EILSEQ or errno == E2BIG if utf16 string is too long),
   *src will point beyond the last successfully converted utf16 character (nul if E2BIG),
 dst != NULL:
   number of utf8 bytes stored, not counting terminating nul:
   a) if terminating nul was stored, *src will be set to NULL, *ps - to initial state,
   b) else (output buffer is too small), *src will point to the next utf16 character to convert.
 dst == NULL:
   size of buffer (in bytes) required to store all utf8 bytes, not counting terminating nul */
size_t utf8_c16srtombs(
	utf8_char_t dst[/*n*/],
	const utf16_char_t **const src,
	size_t n,
	utf8_state_t *ps);

/* mbsrtowcs (3) */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf8 multibyte sequence,
 dst != NULL:
   number of utf32 characters stored, not counting terminating nul:
   a) if terminating nul was stored, *src will be set to NULL,
   b) else (output buffer is too small), *src will point to the next utf8 multibyte sequence to process.
 dst == NULL:
   size of buffer (in utf32-chars) required to store all utf32 characters, not counting terminating nul */
size_t utf8_mbsrtoc32s(
	utf32_char_t dst[/*n*/],
	const utf8_char_t **const src,
	size_t n);

/* wcsrtombs (3) */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf32 character,
 dst != NULL:
   number of utf8 bytes stored, not counting terminating nul:
   a) if terminating nul was stored, *src will be set to NULL, *ps - to initial state,
   b) else (output buffer is too small), *src will point to the next utf32 character to convert.
 dst == NULL:
   size of buffer (in bytes) required to store all utf8 bytes, not counting terminating nul */
size_t utf8_c32srtombs(
	utf8_char_t dst[/*n*/],
	const utf32_char_t **const src,
	size_t n,
	utf8_state_t *ps);

/* mbsrtowcs (3) */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf16 character,
 dst != NULL:
   number of utf32 characters stored, not counting terminating nul:
   a) if terminating nul was stored, *src will be set to NULL,
   b) else (output buffer is too small), *src will point to the next utf16 character to convert.
 dst == NULL:
   size of buffer (in utf32-chars) required to store all utf32 characters, not counting terminating nul */
size_t utf8_c16srtoc32s(
	utf32_char_t dst[/*n*/],
	const utf16_char_t **const src,
	size_t n);

/* wcsrtombs (3) */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf32 character,
 dst != NULL:
   number of utf16 characters stored, not counting terminating nul:
   a) if terminating nul was stored, *src will be set to NULL, *ps - to initial state,
   b) else (output buffer is too small), *src will point to the next utf32 character to convert.
 dst == NULL:
   size of buffer (in utf16-chars) required to store all utf16 characters, not counting terminating nul */
size_t utf8_c32srtoc16s(
	utf16_char_t dst[/*n*/],
	const utf32_char_t **const src,
	size_t n,
	utf8_state_t *ps);

/* ================ non-standard extension ============== */

/* like utf8_mbsrtoc16s(), but do not treats nul characters specially */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf8 multibyte sequence,
 dst != NULL:
   number of utf16 characters stored (including any nul characters):
   a) if the last source utf8 multibyte sequence was processed, *src will be set to NULL,
   b) else (output buffer is too small), *src will point to the next utf8 multibyte sequence to process.
 dst == NULL:
   size of buffer (in utf16-chars) required to store all utf16 characters (including any nul characters) */
/* note: *ps == 0 if it's in initial state */
size_t utf8_mbsrtoc16s_nz(
	utf16_char_t dst[/*n*/],
	const utf8_char_t **const src,
	size_t nsrc,
	size_t n,
	utf8_state_t *ps/*!=NULL*/);

/* like utf8_c16srtombs(), but do not treats nul characters specially */
/* returns:
  -1   on error (errno == EILSEQ or errno == E2BIG if utf16 string is too long),
   *src will point beyond the last successfully converted utf16 character (last character if E2BIG),
 dst != NULL:
   number of utf8 bytes stored (including any nul characters):
   a) if the last source utf16 character was converted, *src will be set to NULL,
   b) else (output buffer is too small), *src will point to the next utf16 character to convert.
 dst == NULL:
   size of buffer (in bytes) required to store all utf8 bytes (including any nul characters) */
/* note: *ps == 0 if it's in initial state */
size_t utf8_c16srtombs_nz(
	utf8_char_t dst[/*n*/],
	const utf16_char_t **const src,
	size_t nsrc,
	size_t n,
	utf8_state_t *ps/*!=NULL*/);

/* like utf8_mbsrtoc32s(), but do not treats nul characters specially */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf8 multibyte sequence,
 dst != NULL:
   number of utf32 characters stored (including any nul characters):
   a) if the last source utf8 multibyte sequence was processed, *src will be set to NULL,
   b) else (output buffer is too small), *src will point to the next utf8 multibyte sequence to process.
 dst == NULL:
   size of buffer (in utf32-chars) required to store all utf32 characters (including any nul characters) */
size_t utf8_mbsrtoc32s_nz(
	utf32_char_t dst[/*n*/],
	const utf8_char_t **const src,
	size_t nsrc,
	size_t n);

/* like utf8_c32srtombs(), but do not treats nul characters specially */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf32 character,
 dst != NULL:
   number of utf8 bytes stored (including any nul characters):
   a) if the last source utf32 character was converted, *src will be set to NULL,
   b) else (output buffer is too small), *src will point to the next utf32 character to convert.
 dst == NULL:
   size of buffer (in bytes) required to store all utf8 bytes (including any nul characters) */
/* note: *ps == 0 if it's in initial state */
size_t utf8_c32srtombs_nz(
	utf8_char_t dst[/*n*/],
	const utf32_char_t **const src,
	size_t nsrc,
	size_t n,
	utf8_state_t *ps/*!=NULL*/);

/* like utf8_c16srtoc32s(), but do not treats nul characters specially */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf16 character,
 dst != NULL:
   number of utf32 characters stored (including any nul characters):
   a) if the last source utf16 character was converted, *src will be set to NULL,
   b) else (output buffer is too small), *src will point to the next utf16 character to convert.
 dst == NULL:
   size of buffer (in utf32-chars) required to store all utf32 characters (including any nul characters) */
/* note: *ps == 0 if it's in initial state */
size_t utf8_c16srtoc32s_nz(
	utf32_char_t dst[/*n*/],
	const utf16_char_t **const src,
	size_t nsrc,
	size_t n);

/* like utf8_c32srtoc16s(), but do not treats nul characters specially */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf32 character,
 dst != NULL:
   number of utf16 characters stored (including any nul characters):
   a) if the last source utf32 character was converted, *src will be set to NULL,
   b) else (output buffer is too small), *src will point to the next utf32 character to convert.
 dst == NULL:
   size of buffer (in utf16-chars) required to store all utf16 characters (including any nul characters) */
/* note: *ps == 0 if it's in initial state */
size_t utf8_c32srtoc16s_nz(
	utf16_char_t dst[/*n*/],
	const utf32_char_t **const src,
	size_t nsrc,
	size_t n,
	utf8_state_t *ps);

#ifdef __cplusplus
}
#endif

#endif /* UTF8_CSTD_H_INCLUDED */
