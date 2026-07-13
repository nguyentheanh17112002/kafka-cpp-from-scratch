#pragma once
#include <string>
#include <vector>
#include "protocol/request.hpp"
#include "common/byte_buffer.hpp"


// struct Topic{
//     int16_t error_code;
//     std::string topic_name;
//     std::array<uint8_t, 16> topic_id;
//     bool is_internal;
    

// }


std::vector<char> handle_describe_topic_partitions(RequestHeader &header, Reader& reader);