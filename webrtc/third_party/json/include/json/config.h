// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_CONFIG_H_INCLUDED
#define JSON_CONFIG_H_INCLUDED
#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>


/// If defined, indicates that json library is embedded in CppTL library.
//# define JSON_IN_CPPTL 1

/// If defined, indicates that json may leverage CppTL library
//#  define JSON_USE_CPPTL 1
/// If defined, indicates that cpptl vector based map should be used instead of
/// std::map
/// as Value container.
//#  define JSON_USE_CPPTL_SMALLMAP 1

// If non-zero, the library uses exceptions to report bad input instead of C
// assertion macros. The default is to use exceptions.
#ifndef JSON_USE_EXCEPTION
#define JSON_USE_EXCEPTION 1
#endif

/// If defined, indicates that the source file is amalgamated
/// to prevent private header inclusion.
/// Remarks: it is automatically defined in the generated amalgamated header.
// #define JSON_IS_AMALGAMATION

#ifdef JSON_IN_CPPTL
#include <cpptl/config.h>
#ifndef JSON_USE_CPPTL
#define JSON_USE_CPPTL 1
#endif
#endif
#define JSON_API
#ifdef JSON_IN_CPPTL
#define JSON_API CPPTL_API
#elif defined(JSON_DLL_BUILD)
#if defined(_MSC_VER) || defined(__MINGW32__)
#define JSON_API __declspec(dllexport)
#define JSONCPP_DISABLE_DLL_INTERFACE_WARNING
#endif // if defined(_MSC_VER)
#elif defined(JSON_DLL)
#if defined(_MSC_VER) || defined(__MINGW32__)
#define JSON_API __declspec(dllimport)
#define JSONCPP_DISABLE_DLL_INTERFACE_WARNING
#endif // if defined(_MSC_VER)
#endif // ifdef JSON_IN_CPPTL
#if !defined(JSON_API)
#define JSON_API __declspec(dllexport)
#endif

#if defined(_MSC_VER) && _MSC_VER < 1800
#error                                                                         \
    "ERROR:  Visual Studio 12 (2013) with _MSC_VER=1800 is the oldest supported compiler with sufficient C++11 capabilities"
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
// As recommended at
// https://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
extern JSON_API int
msvc_pre1900_c99_snprintf(char* outBuf, size_t size, const char* format, ...);
#define jsoncpp_snprintf msvc_pre1900_c99_snprintf
#else
#ifdef __ANDROID__
#define jsoncpp_snprintf snprintf
#else
#define jsoncpp_snprintf std::snprintf
#endif

#endif

// If JSON_NO_INT64 is defined, then Json only support C++ "int" type for
// integer
// Storages, and 64 bits integer support is disabled.
// #define JSON_NO_INT64 1

#if defined(_MSC_VER) // MSVC
#define JSONCPP_DEPRECATED(message) __declspec(deprecated(message))
#endif // defined(_MSC_VER)

// JSONCPP_OVERRIDE is maintained for backwards compatibility of external tools.
// C++11 should be used directly in JSONCPP.
#define JSONCPP_OVERRIDE override

#if __cplusplus >= 201103L
#define JSONCPP_NOEXCEPT noexcept
#define JSONCPP_OP_EXPLICIT explicit
#elif defined(_MSC_VER) && _MSC_VER < 1900
#define JSONCPP_NOEXCEPT throw()
#define JSONCPP_OP_EXPLICIT explicit
#elif defined(_MSC_VER) && _MSC_VER >= 1900
#define JSONCPP_NOEXCEPT noexcept
#define JSONCPP_OP_EXPLICIT explicit
#else
#define JSONCPP_NOEXCEPT throw()
#define JSONCPP_OP_EXPLICIT
#endif

#ifndef JSON_HAS_RVALUE_REFERENCES

#if defined(_MSC_VER)
#define JSON_HAS_RVALUE_REFERENCES 1
#endif // MSVC >= 2013

#ifdef __clang__
#if __has_feature(cxx_rvalue_references)
#define JSON_HAS_RVALUE_REFERENCES 1
#endif // has_feature

#elif defined __GNUC__ // not clang (gcc comes later since clang emulates gcc)
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L)
#define JSON_HAS_RVALUE_REFERENCES 1
#endif // GXX_EXPERIMENTAL

#endif // __clang__ || __GNUC__

#endif // not defined JSON_HAS_RVALUE_REFERENCES

#ifndef JSON_HAS_RVALUE_REFERENCES
#define JSON_HAS_RVALUE_REFERENCES 0
#endif

#ifdef __clang__
#if __has_extension(attribute_deprecated_with_message)
#define JSONCPP_DEPRECATED(message) __attribute__((deprecated(message)))
#endif
#elif defined __GNUC__ // not clang (gcc comes later since clang emulates gcc)
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define JSONCPP_DEPRECATED(message) __attribute__((deprecated(message)))
#elif (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#define JSONCPP_DEPRECATED(message) __attribute__((__deprecated__))
#endif // GNUC version
#endif // __clang__ || __GNUC__

#if !defined(JSONCPP_DEPRECATED)
#define JSONCPP_DEPRECATED(message)
#endif // if !defined(JSONCPP_DEPRECATED)

#if __GNUC__ >= 6
#define JSON_USE_INT64_DOUBLE_CONVERSION 1
#endif

#if !defined(JSON_IS_AMALGAMATION)

#include "allocator.h"
#include "version.h"

#endif // if !defined(JSON_IS_AMALGAMATION)

namespace SJson {
typedef int Int;
typedef unsigned int UInt;
#if defined(JSON_NO_INT64)
typedef int LargestInt;
typedef unsigned int LargestUInt;
#undef JSON_HAS_INT64
#else                 // if defined(JSON_NO_INT64)
// For Microsoft Visual use specific types as long long is not supported
#if defined(_MSC_VER) // Microsoft Visual Studio
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#else                 // if defined(_MSC_VER) // Other platforms, use long long
typedef int64_t Int64;
typedef uint64_t UInt64;
#endif                // if defined(_MSC_VER)
typedef Int64 LargestInt;
typedef UInt64 LargestUInt;
#define JSON_HAS_INT64
#endif // if defined(JSON_NO_INT64)

template <typename T>
using Allocator = typename std::conditional<JSONCPP_USING_SECURE_MEMORY,
                                            SecureAllocator<T>,
                                            std::allocator<T>>::type;
using String = std::basic_string<char, std::char_traits<char>, Allocator<char>>;
using IStringStream = std::basic_istringstream<String::value_type,
                                               String::traits_type,
                                               String::allocator_type>;
using OStringStream = std::basic_ostringstream<String::value_type,
                                               String::traits_type,
                                               String::allocator_type>;
using IStream = std::istream;
using OStream = std::ostream;
} // namespace SJson

// Legacy names (formerly macros).
using JSONCPP_STRING = SJson::String;
using JSONCPP_ISTRINGSTREAM = SJson::IStringStream;
using JSONCPP_OSTRINGSTREAM = SJson::OStringStream;
using JSONCPP_ISTREAM = SJson::IStream;
using JSONCPP_OSTREAM = SJson::OStream;

#endif // JSON_CONFIG_H_INCLUDED
