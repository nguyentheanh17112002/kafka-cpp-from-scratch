#pragma once
#include <cstdint>

enum class APIKey : int16_t {
    PRODUCE = 0,
    FETCH = 1,
    API_VERSIONS = 18,
    DESCRIBE_TOPIC_PARTITIONS = 75
};
