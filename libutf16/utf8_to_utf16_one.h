#ifndef UTF8_TO_UTF16_ONE_H_INCLUDED
#define UTF8_TO_UTF16_ONE_H_INCLUDED

/**********************************************************************************
* UTF-8 -> UTF-16/32 characters conversion
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf8_to_utf16_one.h */

#include "utf16_char.h"

#ifdef __cplusplus
extern "C" {
#endif

/* read one unicode character (code point) from utf8 string, returns:
  (size_t)-1 - s contains invalid/overlong utf8 byte sequence;
  (size_t)-2 - s is too short to read complete unicode character,
               n bytes of s have been consumed, state has been updated,
               need to repeat the call supplying more bytes;
  (size_t)-3 - second part of utf16 surrogate pair of unicode character has
               been read from state, no source bytes have been consumed,
               state has been reset to zero;
  >0         - number of bytes consumed from s, one utf16 character has been read:
               if it's a first part of utf16-surrogate pair, second part has been
               saved in state, otherwise state has been reset to zero;
  0          - nul utf16 character has been read (one nul byte has been consumed from s). */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(pw, A_Notnull)
A_At(s, A_In_reads(n))
A_At(ps, A_Inout)
A_Success(return != A_Size_t(-1))
A_When(return < A_Size_t(-2), A_At(pw, A_Post_valid))
#endif
size_t utf8_to_utf16_one(
	utf16_char_t *const pw/*out,!=NULL*/,
	const utf8_char_t s[/*n*/]/*in,!=NULL*/,
	const size_t n/*>=0*/,
	utf8_state_t *const ps/*in,out,!=NULL*/);

/* read one unicode character (code point) from utf8 string, returns:
  (size_t)-1 - s contains invalid/overlong utf8 byte sequence;
  (size_t)-2 - s is too short to read complete unicode character,
               n bytes of s have been consumed, state has been updated,
               need to repeat the call supplying more bytes;
  >0         - number of bytes consumed from s, one utf32 character has been read,
               state has been reset to zero;
  0          - nul utf32 character has been read (one nul byte has been consumed from s). */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(pw, A_Notnull)
A_At(s, A_In_reads(n))
A_At(ps, A_Inout)
A_Success(return != A_Size_t(-1))
A_When(return < A_Size_t(-2), A_At(pw, A_Post_valid))
#endif
size_t utf8_to_utf32_one(
	utf32_char_t *const pw/*out,!=NULL*/,
	const utf8_char_t s[/*n*/]/*in,!=NULL*/,
	const size_t n/*>=0*/,
	utf8_state_t *const ps/*in,out,!=NULL*/);

/* determine the length of one unicode character (code point) encoded in utf8 string, returns:
  (size_t)-1 - s contains invalid/overlong utf8 byte sequence;
  (size_t)-2 - s is too short to scan complete unicode character,
               n bytes of s have been consumed, state has been updated,
               need to repeat the call supplying more bytes;
  >0         - number of bytes consumed from s, one unicode character has been scanned,
               state has been reset to zero;
  0          - nul unicode character has been scanned (one nul byte has been consumed from s). */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_In_reads(n))
A_At(ps, A_Inout)
A_Success(return != A_Size_t(-1))
#endif
size_t utf8_len_one(
	const utf8_char_t s[/*n*/]/*in,!=NULL*/,
	const size_t n/*>=0*/,
	utf8_state_t *const ps/*in,out,!=NULL*/);

/* read one unicode character (code point) from NUL-terminated utf8 string, returns:
  NULL if s contains invalid or incomplete utf8 byte sequence,
  else - pointer beyond read unicode character stored to (*pw) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(pw, A_Out)
A_At(s, A_In_z)
A_Success(return)
#endif
const utf8_char_t *utf8_to_utf32_one_z(utf32_char_t *const pw/*out,!=NULL*/,
	const utf8_char_t s[]/*'\0'-terminated,!=NULL*/);

#ifdef __cplusplus
}
#endif

#endif /* UTF8_TO_UTF16_ONE_H_INCLUDED */
