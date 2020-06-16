#ifndef UTF16_CHAR_H_INCLUDED
#define UTF16_CHAR_H_INCLUDED

/**********************************************************************************
* UTF-16 <-> UTF-8 characters conversion
* Copyright (C) 2018,2020 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_char.h */

/* utf16 character - 16-bit unsigned integer */
#ifdef _WIN32
typedef wchar_t utf16_char_t;
#else
typedef uint16_t utf16_char_t;
#endif

/* utf32 character - 32-bit unsigned integer */
#ifdef _WIN32
typedef unsigned int utf32_char_t;
#else
typedef uint32_t utf32_char_t;
#endif

/* utf8 character - 8-bit unsigned integer */
typedef unsigned char utf8_char_t;

/* maximum length of utf8-encoded unicode character in bytes */
#define UTF8_MAX_LEN 4

/* utf8 <-> utf16 conversion state */
typedef unsigned int utf8_state_t;

/* check if given utf8_char_t completes a unicode character */
#define utf8_is_one_byte(c) ((c) < 0x80)

/* convert utf8_char_t of one-byte utf8-encoded unicode character to utf16-encoded unicode character */
#define utf8_one_byte_to_utf16(c) ((utf16_char_t)(c))

/* convert utf8_char_t of one-byte utf8-encoded unicode character to utf32-encoded unicode character */
#define utf8_one_byte_to_utf32(c) ((utf32_char_t)(c))

/* check if utf16_char_t is the high part of utf16-surrogate pair */
#define utf16_is_high_surrogate(c) (0xD800 == ((c) & 0xFC00))

/* check if utf16_char_t is the low part of utf16-surrogate pair */
#define utf16_is_low_surrogate(c) (0xDC00 == ((c) & 0xFC00))

/* check if utf16_char_t is a part of utf16-surrogate pair */
#define utf16_is_surrogate(c) (((unsigned)(c) - 0xD800) <= (0xDFFF - 0xD800))

/* h - high part of surrogate pair, l - low part */
#define utf16_to_utf32(h, l) (((unsigned)(h) << 10) + (unsigned)(l) - 0x360DC00 + 0x10000)

/* get high part of utf16 surrogate pair from a utf32 character > 0xFFFF */
#define utf32_get_high_surrogate(u) ((utf16_char_t)((((u) - 0x10000) >> 10) + 0xD800))

/* get low part of utf16 surrogate pair from a utf32 character > 0xFFFF */
#define utf32_get_low_surrogate(u) ((utf16_char_t)((((u) - 0x10000) & 0x3FF) + 0xDC00))

#endif /* UTF16_CHAR_H_INCLUDED */
