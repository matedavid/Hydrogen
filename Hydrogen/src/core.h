#pragma once

#include <cassert>
#include <optional>

#include <spdlog/spdlog.h>

namespace Hydrogen {

#define HG_API __attribute__((visibility("default")))

// Logging & Asserts
#define HG_LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define HG_LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define HG_LOG_ERROR(...) spdlog::error(__VA_ARGS__)

#define HG_ASSERT(condition, ...)  \
    if (!(condition)) {            \
        HG_LOG_ERROR(__VA_ARGS__); \
        assert(false);             \
    }

// Types
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef size_t usize;

// Temporal types
typedef usize ShaderId;

} // namespace Hydrogen