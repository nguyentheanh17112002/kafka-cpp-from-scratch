#pragma once
#include "metadata/metadata_types.hpp"
#include <vector>

std::vector<Topic> parse_metadata_log(const std::vector<char>& bytes);
