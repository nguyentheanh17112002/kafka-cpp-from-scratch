#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include "common/byte_buffer.hpp"

struct RequestHeader{
    int16_t api_key;
    int16_t api_version;
    int32_t correlation_id;
    std::optional<std::string> client_id;
};

struct DescribeTopicPartitionsRequest{
    
};

RequestHeader parse_request_header(Reader& reader);