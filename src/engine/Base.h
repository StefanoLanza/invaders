#pragma once

#include <stdint.h>

#define UNUSED( x ) ( &reinterpret_cast< const size_t& >( x ) )

#ifndef BIT
#define BIT(i) (1u << (unsigned)(i))
#endif

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;
typedef int8_t        int8;
typedef int16_t       int16;
typedef int32_t     int32;
typedef int64_t   int64;
typedef uint8_t        uint8;
typedef uint16_t       uint16;
typedef uint32_t     uint32;
typedef uint64_t   uint64;
typedef float              f32;
typedef double             f64;
typedef uint8_t  byte;
typedef uint16_t word;
typedef uint32_t dword;

#if defined(_MSC_VER)
    //  Microsoft 
    #define DLL_EXPORT __declspec(dllexport)
    #define DLL_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
    //  GCC
    #define DLL_EXPORT __attribute__((visibility("default")))
    #define DLL_IMPORT
#else
    //  do nothing and hope for the best?
    #define DLL_EXPORT
    #define DLL_IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif

#define XMAS_EDITION 1
