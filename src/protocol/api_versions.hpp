#pragma once
#include "protocol/request.hpp"
#include <vector>

std::vector<char> handle_api_versions(RequestHeader& header);
