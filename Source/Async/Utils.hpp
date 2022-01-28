#pragma once

#include <cstdint>

namespace utils {

using unique_id = uint64_t;

auto GenerateTimestampID() -> unique_id;

}
