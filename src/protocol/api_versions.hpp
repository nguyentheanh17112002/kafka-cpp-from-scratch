#pragma once
#include "protocol/request.hpp"
#include <vector>

std::vector<char> handle_api_versions(RequestHeader& header);
struct ApiKeyVersion {
    int16_t api_key;
    int16_t min_version;
    int16_t max_version;
};