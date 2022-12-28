#pragma once

#include <iostream>
#include <cassert>

namespace Hydrogen {

#define HG_API __attribute__((visibility("default")))

#define HG_ASSERT(condition, message) if (!condition) { std::cout << "HG_ASSERT: " << message << "\n"; assert(condition); }

}