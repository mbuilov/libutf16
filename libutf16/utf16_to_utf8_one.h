#ifndef UTF16_TO_UTF8_ONE_H_INCLUDED
#define UTF16_TO_UTF8_ONE_H_INCLUDED

/**********************************************************************************
* UTF-16/32 -> UTF-8 characters conversion
* Copyright (C) 2020-2022 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_to_utf8_one.h */

#include "utf16_char.h"

#ifdef __cplusplus
extern "C" {
#endif

/* write one unicode character (code point) to utf8 string, returns:
  (size_t)-1 - if w is not the second part of utf16 surrogate pair, state not changed;
  0          - w is the first part of utf16 surrogate pair and it has been saved
               in state, no bytes written to s;
  >0         - number of bytes stored to s, state has been reset to zero. */
size_t utf16_to_utf8_one(
	utf8_char_t *LIBUTF16_RESTRICT const s/*[UTF8_MAX_LEN],out,!=NULL*/,
	const utf16_char_t w,
	utf8_state_t *const LIBUTF16_RESTRICT ps/*in,out,!=NULL*/);

/* write one unicode character (code point) to utf8 string, returns:
  (size_t)-1 - if w is not a valid utf32 character (unicode code point)
               (> 0x10FFFF or from surrogate range 0xD800..0xDFFF),
  >0         - number of bytes stored to s. */
size_t utf32_to_utf8_one(
	utf8_char_t s[UTF8_MAX_LEN]/*out,!=NULL*/,
	const utf32_char_t w);

#ifdef __cplusplus
}
#endif

#endif /* UTF16_TO_UTF8_ONE_H_INCLUDED */
