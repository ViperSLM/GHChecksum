#ifndef GH_INTTYPES_H
#define GH_INTTYPES_H

#if defined(__cplusplus)
#include <cstdlib>
typedef size_t usize;
#endif

/* I like this naming scheme for integer types. Don't ask why. */

typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

#ifdef _64BIT_
typedef long long s64;
typedef unsigned long long u64;
#else
typedef s32 s64;
typedef u32 u64;
#endif

#endif