#pragma once
#include <string>
#include <vector>
#include "protocol/request.hpp"
#include "common/byte_buffer.hpp"

std::vector<char> handle_describe_topic_partitions(Reader& reader);