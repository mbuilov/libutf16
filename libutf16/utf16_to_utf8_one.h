#ifndef UTF16_TO_UTF8_ONE_H_INCLUDED
#define UTF16_TO_UTF8_ONE_H_INCLUDED

/**********************************************************************************
* UTF-16 -> UTF-8 characters conversion
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_to_utf8_one.h */

#include "utf16_char.h"

#ifdef __cplusplus
extern "C" {
#endif

/* write one unicode character (code point) to utf8 string, returns:
  (size_t)-1 - if w is not the second part of utf16 surrogate pair;
  0          - w is the first part of utf16 surrogate pair and it has been saved in state;
  >0         - number of bytes stored to s, state has been reset to zero. */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_Pre_writable_size(UTF8_MAX_LEN) A_Post_readable_size(return))
A_At(ps, A_Inout)
A_Ret_range(0,UTF8_MAX_LEN)
A_Success(return <= UTF8_MAX_LEN)
#endif
size_t utf16_to_utf8_one(
	utf8_char_t s[UTF8_MAX_LEN]/*out,!=NULL*/,
	utf16_char_t w,
	utf8_state_t *const ps/*in,out,!=NULL*/);

/* write one unicode character (code point) to utf8 string, returns:
  (size_t)-1 - if w is not a valid utf32 character (>= 0x110000),
  >0         - number of bytes stored to s. */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_Pre_writable_size(UTF8_MAX_LEN) A_Post_readable_size(return))
A_Ret_range(1,UTF8_MAX_LEN)
A_Success(return <= UTF8_MAX_LEN)
#endif
size_t utf32_to_utf8_one(
	utf8_char_t s[UTF8_MAX_LEN]/*out,!=NULL*/,
	utf32_char_t w);

#ifdef __cplusplus
}
#endif

#endif /* UTF16_TO_UTF8_ONE_H_INCLUDED */
