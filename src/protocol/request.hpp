#pragma once
#include <cstdint>
#include "common/byte_buffer.hpp"

struct RequestHeader{
    int16_t api_key;
    int16_t api_version;
    int32_t correlation_id;
};

RequestHeader parse_request_header(Reader& reader);