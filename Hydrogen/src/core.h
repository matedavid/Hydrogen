#pragma once

#include <iostream>
#include <cassert>

#include <spdlog/spdlog.h>

namespace Hydrogen {

#define HG_API __attribute__((visibility("default")))

#define HG_LOG_INFO(...)  spdlog::info(__VA_ARGS__)
#define HG_LOG_WARN(...)  spdlog::warn(__VA_ARGS__)
#define HG_LOG_ERROR(...) spdlog::error(__VA_ARGS__)

#define HG_ASSERT(condition, ...) if (!(condition)) { HG_LOG_ERROR(__VA_ARGS__); exit(-1); }

}