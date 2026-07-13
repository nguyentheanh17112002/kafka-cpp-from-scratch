#pragma once
#include <cstdint>

enum class ErrorCode : int16_t {
    NONE = 0,
    UNKNOWN_TOPIC_OR_PARTITION = 3,
    UNSUPPORTED_VERSION = 35
};
