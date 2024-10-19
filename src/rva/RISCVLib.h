#pragma once
#include <stdint.h>
#include <stdio.h>

typedef int8_t I8;
typedef uint8_t U8;
typedef int16_t I16;
typedef uint16_t U16;
typedef int32_t I32;
typedef uint32_t U32;
typedef int64_t I64;
typedef uint64_t U64;
typedef U64 UPtr;
typedef U8 Byte;
typedef float F32;
typedef double F64;
typedef U8 B8;
typedef U32 B32;
typedef U32 Flags32;

#define U8_MAX 0xFF
#define U16_MAX 0xFFFF
#define U32_MAX 0xFFFFFFFF
#define U64_MAX 0xFFFFFFFFFFFFFFFFULL
#define I8_MAX 0x7F
#define I16_MAX 0x7FFF
#define I32_MAX 0x7FFFFFFF
#define I64_MAX 0x7FFFFFFFFFFFFFFFULL
#define I8_MIN I8(0x80)
#define I16_MIN I16(0x8000)
#define I32_MIN I32(0x80000000)
#define I64_MIN I64(0x8000000000000000LL)
#define B32_TRUE 1
#define B32_FALSE 0
#define B8_TRUE 1
#define B8_FALSE 0

typedef struct {
	const char* str;
	U64 length;
} StrA;

#define StrALit(cstr) (StrA){ cstr, sizeof(cstr) - 1 }

B8 stra_eq(StrA a, StrA b) {
	return a.length == b.length && memcmp(a.str, b.str, a.length) == 0;
}
B8 stra_eq_no_case(StrA a, StrA b) {
	if (a.length != b.length) {
		return B8_FALSE;
	}
	for (U32 i = 0; i < a.length; i++) {
		if ((a.str[i] & ~('a' - 'A')) != (b.str[i] & ~('a' - 'A'))) {
			return B8_FALSE;
		}
	}
	return B8_TRUE;
}
B8 stra_starts_with(StrA str, StrA other) {
	return other.length <= str.length && memcmp(str.str, other.str, other.length) == 0;
}
B8 stra_consume(StrA* str, StrA beginning) {
	if (stra_starts_with(*str, beginning)) {
		str->str += beginning.length;
		str->length -= beginning.length;
		return B8_TRUE;
	}
	return B8_FALSE;
}
B8 srt_is_whitespace(char c) {
	return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v';
}
B8 srt_is_digit(char c) {
	return c >= '0' && c <= '9';
}
B8 srt_is_hex_digit(char c) {
	return c >= '0' && c <= '9' || c >= 'A' && c <= 'F' || c >= 'a' && c <= 'f';
}
U32 srt_hex_digit_to_u32(char c) {
	return (U32)(c >= '0' && c <= '9' ? c - '0' :
		c >= 'A' && c <= 'F' ? c - 'A' + 10 :
		c >= 'a' && c <= 'f' ? c - 'a' + 10 : 0);
}
B8 srt_is_alpha(char c) {
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}
B8 srt_is_alphadigit(char c) {
	return c >= '0' && c <= '9' || c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}
B8 srt_is_upper_alpha(char c) {
	return c >= 'A' && c <= 'Z';
}
B8 srt_is_lower_alpha(char c) {
	return c >= 'a' && c <= 'z';
}
StrA* srt_skip_whitespace(StrA* str) {
	while (str->length && srt_is_whitespace(str->str[0])) {
		str->str++, str->length--;
	}
	return str;
}
enum SRTIntParseError {
	SRT_INT_PARSE_SUCCESS,
	SRT_INT_PARSE_BAD_INPUT,
	SRT_INT_PARSE_OVERFLOW
};

U64 SRT_POWER_OF_10_TABLE[20] = {
	1ULL,
	10ULL,
	100ULL,
	1000ULL,
	10000ULL,
	100000ULL,
	1000000ULL,
	10000000ULL,
	100000000ULL,
	1000000000ULL,
	10000000000ULL,
	100000000000ULL,
	1000000000000ULL,
	10000000000000ULL,
	100000000000000ULL,
	1000000000000000ULL,
	10000000000000000ULL,
	100000000000000000ULL,
	1000000000000000000ULL,
	10000000000000000000ULL,
};

enum SRTIntParseError srt_parse_u64(U64* out, StrA* str) {
	const char* src = str->str;
	U64 srcLen = str->length;
	if (srcLen == 0) {
		return SRT_INT_PARSE_BAD_INPUT;
	}
	U32 base = 10;
	U64 firstOverflow = U64_MAX / 10ull + 1;
	if (srcLen >= 2 && src[0] == '0' && src[1] == 'x') {
		base = 16;
		firstOverflow = 1ull << 60;
		src += 2, srcLen -= 2;
	} else if (srcLen >= 2 && src[0] == '0' && src[1] == 'b') {
		base = 2;
		firstOverflow = 1ull << 63;
		src += 2, srcLen -= 2;
	}
	if (srcLen == 0) {
		return SRT_INT_PARSE_BAD_INPUT;
	}
	U64 num = 0;
	while (srcLen && srt_is_hex_digit(src[0])) {
		if (num >= firstOverflow) {
			return SRT_INT_PARSE_OVERFLOW;
		}
		U64 digit = (U64)(srt_hex_digit_to_u32(src[0]));
		if (digit >= base) {
			break;
		}
		U64 shifted = num * base;
		num = shifted + digit;
		if (num < shifted) {
			return SRT_INT_PARSE_OVERFLOW;
		}
		src++, srcLen--;
	}
	I32 exponent = 0;
	if (srcLen && src[0] == 'e') {
		src++, srcLen--;
		B32 exponentNegative = B8_FALSE;
		if (srcLen && src[0] == '-' || src[0] == '+') {
			exponentNegative = src[0] == '-';
			src++, srcLen--;
		}
		if (!srcLen || !srt_is_digit(src[0])) {
			return SRT_INT_PARSE_BAD_INPUT;
		}
		while (srcLen && srt_is_digit(src[0])) {
			exponent = exponent * 10 + (src[0] - '0');
			if (exponent > 19) {
				return SRT_INT_PARSE_OVERFLOW;
			}
			src++, srcLen--;
		}
		if (exponentNegative) {
			num /= SRT_POWER_OF_10_TABLE[exponent];
		} else {
			U64 exponentiatedNum = num * SRT_POWER_OF_10_TABLE[exponent];
			if (exponentiatedNum / SRT_POWER_OF_10_TABLE[exponent] != num) {
				return SRT_INT_PARSE_OVERFLOW;
			}
			num = exponentiatedNum;
		}
	}
	*out = num;
	str->str = src;
	str->length = srcLen;
	return SRT_INT_PARSE_SUCCESS;
}
B8 stra_consume_u32(U32* result, StrA* str) {
	U64 result64;
	if (srt_parse_u64(&result64, str) == SRT_INT_PARSE_SUCCESS) {
		if (result64 > U32_MAX) {
			return B8_FALSE;
		}
		*result = (U32)(result64);
		return B8_TRUE;
	}
	return B8_FALSE;
}