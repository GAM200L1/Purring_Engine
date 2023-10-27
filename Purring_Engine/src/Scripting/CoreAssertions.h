#pragma once
#include <iostream>
#include <string>

// Define a debug break function. You can use built-in functions for Windows or Linux.
#if defined(_MSC_VER)
#define DEBUGBREAK() __debugbreak()
#elif defined(__GNUC__)
#include <signal.h>
#define DEBUGBREAK() raise(SIGTRAP)
#else
#define DEBUGBREAK()
#endif

// Enable assertion checks only when DEBUG mode is active
#if defined(DEBUG) || defined(_DEBUG)

    // Internal function to perform the assertion logic
#define INTERNAL_ASSERT_IMPL(check, msg) { \
        if (!(check)) { \
            std::cerr << "Assertion failed: " << msg << std::endl; \
            DEBUGBREAK(); \
        } \
    }

    // For now, directly use INTERNAL_ASSERT_IMPL
#define CORE_ASSERT(...) INTERNAL_ASSERT_IMPL(__VA_ARGS__)

#else
    // If not in debug mode, define it as an empty macro
#define CORE_ASSERT(...)
#endif
