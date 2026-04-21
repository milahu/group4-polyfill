#pragma once

// TODO?
#define BUILDFLAG(X) defined(X)

// ---- OS ----
#if defined(_WIN32)
#define IS_WIN 1
#elif defined(__APPLE__)
#define IS_APPLE 1
#elif defined(__linux__)
#define IS_LINUX 1
#else
#define IS_POSIX 1
#endif

// ---- ARCH ----
#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_CPU_X86_64 1
#define ARCH_CPU_X86_FAMILY 1
#elif defined(__aarch64__)
#define ARCH_CPU_ARM64 1
// #elif TODO
// #define ARCH_CPU_ARMEL 1
#endif

// ---- COMPILER ----
#if defined(__clang__)
#define COMPILER_CLANG 1
#pragma message "compiler=clang"
#error pdfium requires GCC. see "No supported trap sequence!" in src/third_party/pdfium/core/fxcrt/immediate_crash.h
#elif defined(__GNUC__)
#define COMPILER_GCC 1
// #pragma message "compiler=gcc"
#elif defined(_MSC_VER)
#define COMPILER_MSVC 1
#pragma message "compiler=msvc"
#error pdfium requires GCC. see "No supported trap sequence!" in src/third_party/pdfium/core/fxcrt/immediate_crash.h
#endif

// #define _HAS_ITERATOR_DEBUGGING 0
#define _GLIBCXX_USE_CXX11_ABI 1
#define _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
