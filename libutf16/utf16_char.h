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

#endif /* UTF16_CHAR_H_INCLUDED */
