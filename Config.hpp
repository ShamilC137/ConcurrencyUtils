#ifndef APPLICATION_CONFIG_HPP_
#define APPLICATION_CONFIG_HPP_

// Description:
// this file contains macroses that must be used in program;
// using of dependent macro (by OS, compiler, etc) is unacceptable
// if unacceptable configuration is used, panic message will be arised

// project specific macro:
// flags:
// STL_ALLOCATOR_USAGE - if enabled, makes containers use the STL allocator
// VALLOCATOR_USAGE - if enabled, makes containers use the virtual allocator

// compiler macro:
// MSVC - means that MSC is used
// GNUCPP - means that g++ is used
// CLANG - means that clang is used (panic now, will not compile)

// OS specific macro:
// WINDOWS32 - Windows 32 bit is used
// WINDOWS64 - Windows 64 bit is used

// compiler determination vvv
#ifdef _MSC_VER
#define MSVC 1
#elif defined _GNUG_
#define GNUCPP 1
#elif __clang__
#define CLANG 1
static_assert(false, "Panic: clang")
#endif
// compiler determination ^^^

// start OS resolution vvv
#if _WIN32 | _WIN64
#if _WIN32
#define WINDOWS32 1
#else
#define WINDOWS64 1
#endif // !_WIN32
#else
static_assert(false, "Panic: unsupported OS");
#endif // !_WIN32 | _WIN64
// OS resolution ^^^

// this project specific macro vvv
// allocator resolution flags vvv
// flags' arguments: 1 if enabled, 0 otherwise
// if some flags are enabled at the same time, the topmost enabled flag will be
// taken as active
// if 1 all aliased containers must use standard allocator;
#define STL_ALLOCATOR_USAGE 1
// if 1 all aliased containers must use standard allocator
#define VALLOCATOR_USAGE 0
// allocator resolution flags ^^^
// this project specific macro ^^^

#endif // !APPLICATION_CONFIG_HPP_
