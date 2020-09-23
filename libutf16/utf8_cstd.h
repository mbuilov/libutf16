#ifndef UTF8_CSTD_H_INCLUDED
#define UTF8_CSTD_H_INCLUDED

/**********************************************************************************
* C90/C99/C11 standard multibyte-to-widechar conversion routines for UTF-8 encoding
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf8_cstd.h */

#include "utf16_char.h"

#ifdef __cplusplus
extern "C" {
#endif

/* mblen (3) */
/* returns:
  -1   on incomplete character (errno == 0) or on error (errno == EILSEQ)
 s != NULL:
   0   for nul character (1 byte read),
   1-4 bytes read
 s == NULL:
   0   UTF8 is a stateless encoding */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_In_reads_opt(n))
A_Success(return != -1)
#endif
int utf8_mblen(const utf8_char_t *const s, const size_t n);

/* mbrlen (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 s != NULL:
  -2   on incomplete character,
   0   for nul character (1 byte read),
   1-4 bytes read
 s == NULL:
   0   state is zero */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbrlen(const utf8_char_t *const s, const size_t n, utf8_state_t *ps);

/* mbrtoc16 (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 s != NULL:
  -3   second part of utf16 surrogate pair has been read from state,
       no source bytes have been consumed,
  -2   on incomplete character,
   0   for nul character (1 byte read),
   1-4 bytes read
 s == NULL:
   0   state is zero */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_When(s, A_At(pwc, A_Out_opt))
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbrtoc16(utf16_char_t *const pwc, const utf8_char_t *const s,
	const size_t n, utf8_state_t *ps);

/* mbrtoc32 (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 s != NULL:
  -2   on incomplete character,
   0   for nul character (1 byte read),
   1-4 bytes read
 s == NULL:
   0   state is zero */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_When(s, A_At(pwi, A_Out_opt))
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbrtoc32(utf32_char_t *const pwi, const utf8_char_t *const s,
	const size_t n, utf8_state_t *ps);

/* c16rtomb (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 s != NULL:
   0   high utf16-surrogate has been saved in state, no bytes written,
   1-4 bytes written
 s == NULL:
   1   one byte needed to encode nul character */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_Out_writes_opt(UTF8_MAX_LEN))
A_At(ps, A_Inout_opt)
A_Ret_range(0, UTF8_MAX_LEN)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c16rtomb(utf8_char_t *const s, const utf16_char_t wc, utf8_state_t *ps);

/* c32rtomb (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 s != NULL:
   1-4 bytes written
 s == NULL:
   1   one byte needed to encode nul character */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_Out_writes_opt(UTF8_MAX_LEN))
A_At(ps, A_In_opt)
A_Ret_range(1, UTF8_MAX_LEN)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c32rtomb(utf8_char_t *const s, const utf32_char_t wi, utf8_state_t *ps);

/* mbtowc (3) for 16/32 bit wchar_t */
/* returns:
  -1   on incomplete character (errno == 0) or on error (errno == EILSEQ)
 s != NULL:
   0   for nul character (1 byte read),
   1-4 bytes read
 s == NULL:
   0   UTF8 is a stateless encoding */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_When(s, A_At(pwc, A_Out_opt))
A_At(s, A_In_reads_opt(n))
A_Ret_range(-1, UTF8_MAX_LEN)
#endif
/* NOTE: do not handles utf16 surrogate pairs! Use utf8_mbrtoc16 instead! */
int utf8_mbtowc16_obsolete(utf16_char_t *const pwc, const utf8_char_t *const s, const size_t n);

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_When(s, A_At(pwc, A_Out_opt))
A_At(s, A_In_reads_opt(n))
A_Ret_range(-1, UTF8_MAX_LEN)
#endif
int utf8_mbtowc32(utf32_char_t *const pwc, const utf8_char_t *const s, const size_t n);

/* mbrtowc (3) for 16/32 bit wchar_t */
/* returns:
  -1   on error (errno == EILSEQ)
 s != NULL:
  -2   on incomplete character,
   0   for nul character (1 byte read),
   1-4 bytes read
 s == NULL:
   0   state is zero */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_When(s, A_At(pwc, A_Out_opt))
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
/* NOTE: do not handles utf16 surrogate pairs! Use utf8_mbrtoc16 instead! */
size_t utf8_mbrtowc16_obsolete(utf16_char_t *const pwc, const utf8_char_t *const s,
	const size_t n, utf8_state_t *ps);

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_When(s, A_At(pwc, A_Out_opt))
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbrtowc32(utf32_char_t *const pwc, const utf8_char_t *const s,
	const size_t n, utf8_state_t *ps);

/* wctomb (3) for 16/32 bit wchar_t */
/* returns:
  -1   on error (errno == EILSEQ)
 s != NULL:
   1-4 bytes written
 s == NULL:
   0   UTF8 is a stateless encoding */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_Out_writes_opt(UTF8_MAX_LEN))
A_Ret_range(0, UTF8_MAX_LEN)
A_Success(return != -1)
#endif
/* NOTE: do not handles utf16 surrogate pairs! Use utf8_c16rtomb instead! */
int utf8_wc16tomb_obsolete(utf8_char_t *const s, const utf16_char_t wc);

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_Out_writes_opt(UTF8_MAX_LEN))
A_Ret_range(0, UTF8_MAX_LEN)
A_Success(return != -1)
#endif
int utf8_wc32tomb(utf8_char_t *const s, const utf32_char_t wc);

/* wcrtomb (3) for 16/32 bit wchar_t */
/* returns:
  -1   on error (errno == EILSEQ)
 s != NULL:
   1-4 bytes written
 s == NULL:
   1   one byte needed to encode nul character */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_Out_writes_opt(UTF8_MAX_LEN))
A_At(ps, A_Inout_opt)
A_Ret_range(1, UTF8_MAX_LEN)
A_Success(return != A_Size_t(-1))
#endif
/* NOTE: do not handles utf16 surrogate pairs! Use utf8_c16rtomb instead! */
size_t utf8_wc16rtomb_obsolete(utf8_char_t *const s, const utf16_char_t wc, utf8_state_t *ps);

#define utf8_wc32rtomb utf8_c32rtomb

/* ================ mbstowcs()/wcstombs() ============== */

/* mbstowcs (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 dst != NULL:
   number of utf16 characters stored, not counting terminating nul, which is not written
   if buffer is too small. Use red zone in output buffer of at least 2 utf16 characters
   to check if buffer size is enough to store all characters, including terminating nul
   character (only nul character can be written to red zone).
 dst == NULL:
   size of buffer (in utf16-chars) required to store all utf16 characters, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(dst, A_Out_writes_opt(n))
A_At(src, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbstoc16s(utf16_char_t dst[/*n*/], const utf8_char_t *const src, size_t n);

/* wcstombs (3) */
/* returns:
  -1   on error (errno == EILSEQ or errno == E2BIG if utf16 string is too long)
 dst != NULL:
   number of utf8 bytes stored, not counting terminating nul, which is not written
   if buffer is too small. Use red zone in output buffer of at least UTF8_MAX_LEN
   utf8 bytes to check if buffer size is enough to store all characters, including
   terminating nul character (only nul character can be written to red zone).
 dst == NULL:
   size of buffer (in bytes) required to store all utf8 bytes, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(dst, A_Out_writes_opt(n))
A_At(src, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c16stombs(utf8_char_t dst[/*n*/], const utf16_char_t *const src, size_t n);

/* mbstowcs (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 dst != NULL:
   number of utf32 characters stored, not counting terminating nul, which is not written
   if buffer is too small. Use red zone in output buffer of at least 1 utf32 character
   to check if buffer size is enough to store all characters, including terminating nul
   character (only nul character can be written to red zone).
 dst == NULL:
   size of buffer (in utf32-chars) required to store all utf32 characters, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(dst, A_Out_writes_opt(n))
A_At(src, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbstoc32s(utf32_char_t dst[/*n*/], const utf8_char_t *const src, size_t n);

/* wcstombs (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 dst != NULL:
   number of utf8 bytes stored, not counting terminating nul, which is not written
   if buffer is too small. Use red zone in output buffer of at least UTF8_MAX_LEN
   utf8 bytes to check if buffer size is enough to store all characters, including
   terminating nul character (only nul character can be written to red zone).
 dst == NULL:
   size of buffer (in bytes) required to store all utf8 bytes, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(dst, A_Out_writes_opt(n))
A_At(src, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c32stombs(utf8_char_t dst[/*n*/], const utf32_char_t *const src, size_t n);

/* mbstowcs (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 dst != NULL:
   number of utf32 characters stored, not counting terminating nul, which is not written
   if buffer is too small. Use red zone in output buffer of at least 1 utf32 character
   to check if buffer size is enough to store all characters, including terminating nul
   character (only nul character can be written to red zone).
 dst == NULL:
   size of buffer (in utf32-chars) required to store all utf32 characters, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(dst, A_Out_writes_opt(n))
A_At(src, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c16stoc32s(utf32_char_t dst[/*n*/], const utf16_char_t *const src, size_t n);

/* wcstombs (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 dst != NULL:
   number of utf16 characters stored, not counting terminating nul, which is not written
   if buffer is too small. Use red zone in output buffer of at least 2 utf16 characters
   to check if buffer size is enough to store all characters, including terminating nul
   character (only nul character can be written to red zone).
 dst == NULL:
   size of buffer (in utf16-chars) required to store all utf16 characters, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(dst, A_Out_writes_opt(n))
A_At(src, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c32stoc16s(utf16_char_t dst[/*n*/], const utf32_char_t *const src, size_t n);

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
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(src, A_Inout A_Outptr_result_maybenull_z)
A_At(*src, A_In_z)
A_At(dst, A_Out_writes_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbsrtoc16s(utf16_char_t dst[/*n*/], const utf8_char_t **const src, size_t n, utf8_state_t *ps);

/* wcsrtombs (3) */
/* returns:
  -1   on error (errno == EILSEQ or errno == E2BIG if utf16 string is too long),
   *src will point beyond the last successfully converted utf16 character,
 dst != NULL:
   number of utf8 bytes stored, not counting terminating nul:
   a) if terminating nul was stored, *src will be set to NULL, *ps - to initial state,
   b) else (output buffer is too small), *src will point to the next utf16 character to convert.
 dst == NULL:
   size of buffer (in bytes) required to store all utf8 bytes, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(src, A_Inout A_Outptr_result_maybenull_z)
A_At(*src, A_In_z)
A_At(dst, A_Out_writes_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c16srtombs(utf8_char_t dst[/*n*/], const utf16_char_t **const src, size_t n, utf8_state_t *ps);

/* mbsrtowcs (3) */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf8 multibyte sequence,
 dst != NULL:
   number of utf32 characters stored, not counting terminating nul:
   a) if terminating nul was stored, *src will be set to NULL,
   b) else (output buffer is too small), *src will point to the next utf8 multibyte sequence to process.
 dst == NULL:
   size of buffer (in utf32-chars) required to store all utf32 characters, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(src, A_Inout A_Outptr_result_maybenull_z)
A_At(*src, A_In_z)
A_At(dst, A_Out_writes_opt(n))
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbsrtoc32s(utf32_char_t dst[/*n*/], const utf8_char_t **const src, size_t n);

/* wcsrtombs (3) */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf32 character,
 dst != NULL:
   number of utf8 bytes stored, not counting terminating nul:
   a) if terminating nul was stored, *src will be set to NULL, *ps - to initial state,
   b) else (output buffer is too small), *src will point to the next utf32 character to convert.
 dst == NULL:
   size of buffer (in bytes) required to store all utf8 bytes, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(src, A_Inout A_Outptr_result_maybenull_z)
A_At(*src, A_In_z)
A_At(dst, A_Out_writes_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c32srtombs(utf8_char_t dst[/*n*/], const utf32_char_t **const src, size_t n, utf8_state_t *ps);

/* mbsrtowcs (3) */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf16 character,
 dst != NULL:
   number of utf32 characters stored, not counting terminating nul:
   a) if terminating nul was stored, *src will be set to NULL,
   b) else (output buffer is too small), *src will point to the next utf16 character to convert.
 dst == NULL:
   size of buffer (in utf32-chars) required to store all utf32 characters, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(src, A_Inout A_Outptr_result_maybenull_z)
A_At(*src, A_In_z)
A_At(dst, A_Out_writes_opt(n))
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c16srtoc32s(utf32_char_t dst[/*n*/], const utf16_char_t **const src, size_t n);

/* wcsrtombs (3) */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf32 character,
 dst != NULL:
   number of utf16 characters stored, not counting terminating nul:
   a) if terminating nul was stored, *src will be set to NULL, *ps - to initial state,
   b) else (output buffer is too small), *src will point to the next utf32 character to convert.
 dst == NULL:
   size of buffer (in utf16-chars) required to store all utf16 characters, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(src, A_Inout A_Outptr_result_maybenull_z)
A_At(*src, A_In_z)
A_At(dst, A_Out_writes_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c32srtoc16s(utf16_char_t dst[/*n*/], const utf32_char_t **const src, size_t n, utf8_state_t *ps);

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
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_Nonnull_arg(5)
A_At(src, A_Inout A_Outptr_result_maybenull)
A_At(*src, A_In_reads(nsrc))
A_At(dst, A_Out_writes_opt(n))
A_At(dst, A_Post_readable_size(return))
A_At(ps, A_Inout)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbsrtoc16s_nz(utf16_char_t dst[/*n*/], const utf8_char_t **const src,
	size_t nsrc, size_t n, utf8_state_t *ps/*!=NULL*/);

/* like utf8_c16srtombs(), but do not treats nul characters specially */
/* returns:
  -1   on error (errno == EILSEQ or errno == E2BIG if utf16 string is too long),
   *src will point beyond the last successfully converted utf16 character,
 dst != NULL:
   number of utf8 bytes stored (including any nul characters):
   a) if the last source utf16 character was converted, *src will be set to NULL,
   b) else (output buffer is too small), *src will point to the next utf16 character to convert.
 dst == NULL:
   size of buffer (in bytes) required to store all utf8 bytes (including any nul characters) */
/* note: *ps == 0 if it's in initial state */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_Nonnull_arg(5)
A_At(src, A_Inout A_Outptr_result_maybenull)
A_At(*src, A_In_reads(nsrc))
A_At(dst, A_Out_writes_opt(n))
A_At(dst, A_Post_readable_size(return))
A_At(ps, A_Inout)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c16srtombs_nz(utf8_char_t dst[/*n*/], const utf16_char_t **const src,
	size_t nsrc, size_t n, utf8_state_t *ps/*!=NULL*/);

/* like utf8_mbsrtoc32s(), but do not treats nul characters specially */
/* returns:
  -1   on error (errno == EILSEQ), *src will point to invalid utf8 multibyte sequence,
 dst != NULL:
   number of utf32 characters stored (including any nul characters):
   a) if the last source utf8 multibyte sequence was processed, *src will be set to NULL,
   b) else (output buffer is too small), *src will point to the next utf8 multibyte sequence to process.
 dst == NULL:
   size of buffer (in utf32-chars) required to store all utf32 characters (including any nul characters) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(src, A_Inout A_Outptr_result_maybenull)
A_At(*src, A_In_reads(nsrc))
A_At(dst, A_Out_writes_opt(n))
A_At(dst, A_Post_readable_size(return))
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbsrtoc32s_nz(utf32_char_t dst[/*n*/], const utf8_char_t **const src,
	size_t nsrc, size_t n);

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
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_Nonnull_arg(5)
A_At(src, A_Inout A_Outptr_result_maybenull)
A_At(*src, A_In_reads(nsrc))
A_At(dst, A_Out_writes_opt(n))
A_At(dst, A_Post_readable_size(return))
A_At(ps, A_Inout)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c32srtombs_nz(utf8_char_t dst[/*n*/], const utf32_char_t **const src,
	size_t nsrc, size_t n, utf8_state_t *ps/*!=NULL*/);

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
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(src, A_Inout A_Outptr_result_maybenull)
A_At(*src, A_In_reads(nsrc))
A_At(dst, A_Out_writes_opt(n))
A_At(dst, A_Post_readable_size(return))
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c16srtoc32s_nz(utf32_char_t dst[/*n*/], const utf16_char_t **const src,
	size_t nsrc, size_t n);

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
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_Nonnull_arg(5)
A_At(src, A_Inout A_Outptr_result_maybenull)
A_At(*src, A_In_reads(nsrc))
A_At(dst, A_Out_writes_opt(n))
A_At(dst, A_Post_readable_size(return))
A_At(ps, A_Inout)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c32srtoc16s_nz(utf16_char_t dst[/*n*/], const utf32_char_t **const src,
	size_t nsrc, size_t n, utf8_state_t *ps);

#ifdef __cplusplus
}
#endif

#endif /* UTF8_CSTD_H_INCLUDED */
