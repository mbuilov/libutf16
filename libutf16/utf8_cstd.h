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
A_Ret_range(1, UTF8_MAX_LEN)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_c32rtomb(utf8_char_t *const s, const utf32_char_t wi);

/* mbtowc (3) */
/* returns:
  -1   on incomplete character (errno == 0) or on error (errno == EILSEQ)
 s != NULL:
   0   for nul character (1 byte read),
   1-4 bytes read
 s == NULL:
   0   UTF8 is a stateless encoding */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(pwc, A_Out_opt)
A_At(s, A_In_reads_opt(n))
A_Ret_range(-1, UTF8_MAX_LEN)
#endif
int utf8_mbtowc16(utf16_char_t *const pwc, const utf8_char_t *const s, const size_t n);

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(pwc, A_Out_opt)
A_At(s, A_In_reads_opt(n))
A_Ret_range(-1, UTF8_MAX_LEN)
#endif
int utf8_mbtowc32(utf32_char_t *const pwc, const utf8_char_t *const s, const size_t n);

/* mbrtowc (3) */
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
A_At(pwc, A_Out_opt)
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbrtowc16(utf16_char_t *const pwc, const utf8_char_t *const s,
	const size_t n, utf8_state_t *const ps);

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(pwc, A_Out_opt)
A_At(s, A_In_reads_opt(n))
A_At(ps, A_Inout_opt)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbrtowc32(utf32_char_t *const pwc, const utf8_char_t *const s,
	const size_t n, utf8_state_t *ps);

/* wctomb (3) */
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
int utf8_wc16tomb(utf8_char_t *const s, const utf16_char_t wc);

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_Out_writes_opt(UTF8_MAX_LEN))
A_Ret_range(0, UTF8_MAX_LEN)
A_Success(return != -1)
#endif
int utf8_wc32tomb(utf8_char_t *const s, const utf32_char_t wc);

/* wcrtomb (3) */
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
size_t utf8_wc16rtomb(utf8_char_t *const s, const utf16_char_t wc, utf8_state_t *ps);

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(s, A_Out_writes_opt(UTF8_MAX_LEN))
A_At(ps, A_Inout_opt)
A_Ret_range(1, UTF8_MAX_LEN)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_wc32rtomb(utf8_char_t *const s, const utf32_char_t wc, utf8_state_t *ps);

/* mbstowcs (3) */
/* returns:
  -1   on error (errno == EILSEQ)
 wcstr != NULL:
   number of wide characters stored, not counting terminating nul, which is not written
   if buffer is too small. Use red zone in output buffer of at least 2 utf16 characters
   to check if buffer size is enough to store all characters, including terminating nul
   character (only nul character can be written to red zone).
 wcstr == NULL:
   size of buffer required to store all wide characters, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(wcstr, A_Out_writes_opt(count))
A_At(mbstr, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_mbstowc16s(utf16_char_t *const wcstr, const utf8_char_t *const mbstr, size_t count);

/* wcstombs (3) */
/* returns:
  -1   on error (errno == EILSEQ or errno == E2BIG if utf16 string is too long)
 mbstr != NULL:
   number of utf8 bytes stored, not counting terminating nul, which is not written
   if buffer is too small. Use red zone in output buffer of at least UTF8_MAX_LEN
   utf8 bytes to check if buffer size is enough to store all characters, includings
   terminating nul character (only nul character can be written to red zone).
 mbstr == NULL:
   size of buffer required to store all utf8 bytes, not counting terminating nul */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(mbstr, A_Out_writes_opt(count))
A_At(wcstr, A_In_z)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_wc16stombs(utf8_char_t *const mbstr, const utf16_char_t *const wcstr, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* UTF8_CSTD_H_INCLUDED */
