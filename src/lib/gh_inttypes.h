#ifndef GH_INTTYPES_H
#define GH_INTTYPES_H
#include <cstdint>

/* I like this naming scheme for integer types. Don't ask why. */

typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef size_t    usize;

typedef intptr_t  sptr;
typedef uintptr_t uptr;

#endif