#ifndef UTF16_CHAR_H_INCLUDED
#define UTF16_CHAR_H_INCLUDED

/**********************************************************************************
* UTF-16 <-> UTF-8 characters conversion
* Copyright (C) 2018-2022 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_char.h */

#if 0

Valid character ranges in utf encodings.

Utf-8 encoding:

 byte 1   byte 2   byte 3   byte 4  | min/max ranges           | code point
------------------------------------+--------------------------+-----------
00000000                            |                        0 | 0x00
01111111                            |                  1111111 | 0x7F
10000000                            |   invalid (continuation) |
11000000 10000000                   |       invalid (overlong) |
11000001 10111111                   |       invalid (overlong) |
11000010 10000000                   |                 10000000 | 0x80
11011111 10111111                   |              11111111111 | 0x7FF
11100000 10000000 10000000          |       invalid (overlong) |
11100000 10011111 10111111          |       invalid (overlong) |
11100000 10100000 10000000          |             100000000000 | 0x800
11101101 10011111 10111111          |         1101011111111111 | 0xD7FF
11101101 10100000 10000000          | invalid (high surrogate) |
11101101 10111111 10111111          |  invalid (low surrogate) |
11101110 10000000 10000000          |         1110000000000000 | 0xE000
11101111 10111111 10111111          |         1111111111111111 | 0xFFFF
11110000 10000000 10000000 10000000 |       invalid (overlong) |
11110000 10001111 10111111 10111111 |       invalid (overlong) |
11110000 10010000 10000000 10000000 |        10000000000000000 | 0x10000
11110100 10001111 10111111 10111111 |    100001111111111111111 | 0x10FFFF
11110100 10010000 10000000 10000000 |        invalid (too big) |
11111111 ...                        |        invalid (too big) |
------------------------------------+--------------------------------------

Utf-16 encoding:

 word1 1          word 2            | min/max ranges           | code point
------------------------------------+--------------------------+-----------
0000000000000000                    |                        0 | 0x00
1101011111111111                    |         1101011111111111 | 0xD7FF
1101100000000000 1101110000000000   |        10000000000000000 | 0x10000
1101101111111111 1101111111111111   |    100001111111111111111 | 0x10FFFF
1101110000000000                    |  invalid (low surrogate) |
1101111111111111                    |  invalid (low surrogate) |
1110000000000000                    |         1110000000000000 | 0xE000
1111111111111111                    |         1111111111111111 | 0xFFFF
------------------------------------+--------------------------+-----------

Utf-32 encoding:

 dword 1                            | min/max ranges           | code point
------------------------------------+--------------------------+-----------
00000000000000000000000000000000    |                        0 | 0x00
00000000000000001101011111111111    |         1101011111111111 | 0xD7FF
00000000000000001101100000000000    |      invalid (surrogate) | 0xD800
00000000000000001101111111111111    |      invalid (surrogate) | 0xDFFF
00000000000000001110000000000000    |         1110000000000000 | 0xE000
00000000000100001111111111111111    |    100001111111111111111 | 0x10FFFF
00000000000100010000000000000000    |        invalid (too big) | 0x110000
11111111111111111111111111111111    |        invalid (too big) | 0xFFFFFFFF
------------------------------------+--------------------------+-----------

#endif

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

/* to declare pointers to unaligned buffers containing utf16/utf32 characters */
typedef unsigned char utf16_char_unaligned_t[sizeof(utf16_char_t)];
typedef unsigned char utf32_char_unaligned_t[sizeof(utf32_char_t)];

/* utf8 <-> utf16 conversion state */
typedef unsigned int utf8_state_t;

/* decode length of utf8 character encoded in first byte */
/* note: assume given byte is valid, valid ranges for the first byte are: [0..0x7F] or [0xC2..0xF4] */
/* note: c is used multiple times! */
#define utf8_decode_len(c) (1 + ((c) >= 0xC0) + ((c) >= 0xE0) + ((c) >= 0xF0))

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

/* convert utf16 surrogate pair to a utf32 character,
  h - high part of surrogate pair, l - low part */
#define utf16_pair_to_utf32_one(h, l) (((utf32_char_t)(h) << 10) + (unsigned)(l) - 0x360DC00 + 0x10000)

/* get high part of utf16 surrogate pair from a utf32 character > 0xFFFF */
#define utf32_get_high_surrogate(u) ((utf16_char_t)((((u) - 0x10000) >> 10) + 0xD800))

/* get low part of utf16 surrogate pair from a utf32 character > 0xFFFF */
#define utf32_get_low_surrogate(u) ((utf16_char_t)((((u) - 0x10000) & 0x3FF) + 0xDC00))

/* maximum value of utf32_char_t */
#define UTF32_MAX_VALUE 0x10FFFF

/* check if utf32_char_t is in a range reserved for utf16 surrogates */
#define utf32_is_surrogate(c) (((c) - 0xD800) <= (0xDFFF - 0xD800))

/* utf16/utf32 character used as Byte Order Mark (BOM) */
#define UTF16_BOM 0xFEFF

/* check for BOM in array of bytes */
#define UTF16_BYTES_IS_BOM_LE(a,b)       ((a) == 0xFF && (b) == 0xFE)
#define UTF16_BYTES_IS_BOM_BE(a,b)       ((a) == 0xFE && (b) == 0xFF)
#define UTF32_BYTES_IS_BOM_LE(a,b,c,d)   ((a) == 0xFF && (b) == 0xFE && (c) == 0 && (d) == 0)
#define UTF32_BYTES_IS_BOM_BE(a,b,c,d)   ((a) == 0 && (b) == 0 && (c) == 0xFE && (d) == 0xFF)

/* check if p points to bytes of BOM */
/* note: p is used multiple times! */
#define UTF16_IS_BOM_LE(p)   UTF16_BYTES_IS_BOM_LE(((const unsigned char*)(p))[0], \
                                                   ((const unsigned char*)(p))[1])

#define UTF16_IS_BOM_BE(p)   UTF16_BYTES_IS_BOM_BE(((const unsigned char*)(p))[0], \
                                                   ((const unsigned char*)(p))[1])

#define UTF32_IS_BOM_LE(p)   UTF32_BYTES_IS_BOM_LE(((const unsigned char*)(p))[0], \
                                                   ((const unsigned char*)(p))[1], \
                                                   ((const unsigned char*)(p))[2], \
                                                   ((const unsigned char*)(p))[3])

#define UTF32_IS_BOM_BE(p)   UTF32_BYTES_IS_BOM_BE(((const unsigned char*)(p))[0], \
                                                   ((const unsigned char*)(p))[1], \
                                                   ((const unsigned char*)(p))[2], \
                                                   ((const unsigned char*)(p))[3])

/* check for BOM in a valid utf8 string: b and c are evaluated only when needed */
#define UTF8_IS_BOM(a,b,c)   ((a) == 0xEF && (b) == 0xBB && (c) == 0xBF)

/* try to workaround gcc bug
  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81631 */
#if defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#define const_utf16_char_unaligned_ptr(p_) __extension__({ \
  _Pragma("GCC diagnostic push") \
  _Pragma("GCC diagnostic ignored \"-Wcast-qual\"") \
  const utf16_char_unaligned_t *const c_ = (const utf16_char_unaligned_t*)(p_); \
  _Pragma("GCC diagnostic pop") \
  c_; \
})
#define const_utf32_char_unaligned_ptr(p_) __extension__({ \
  _Pragma("GCC diagnostic push") \
  _Pragma("GCC diagnostic ignored \"-Wcast-qual\"") \
  const utf32_char_unaligned_t *const c_ = (const utf32_char_unaligned_t*)(p_); \
  _Pragma("GCC diagnostic pop") \
  c_; \
})
#else /* !__GNUC__ */
#define const_utf16_char_unaligned_ptr(p_)     ((const utf16_char_unaligned_t*)(p_))
#define const_utf32_char_unaligned_ptr(p_)     ((const utf32_char_unaligned_t*)(p_))
#endif /* !__GNUC__ */

/* LIBUTF16_RESTRICT - annotates a pointer though which:
  - reads may be prefetched,
  - writes may be delayed */
#ifndef LIBUTF16_RESTRICT
#ifdef A_Restrict
#define LIBUTF16_RESTRICT A_Restrict
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#define LIBUTF16_RESTRICT restrict
#elif defined(_MSC_VER) && (_MSC_VER >= 1600)
#define LIBUTF16_RESTRICT __restrict
#elif defined(__GNUC__) && (__GNUC__ >= 3)
#define LIBUTF16_RESTRICT __restrict__
#elif defined(__clang__)
#define LIBUTF16_RESTRICT __restrict__
#else
#define LIBUTF16_RESTRICT
#endif
#endif

#endif /* UTF16_CHAR_H_INCLUDED */
