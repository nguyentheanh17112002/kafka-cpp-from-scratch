#pragma once
#include <vector>
#include "common/byte_buffer.hpp"
#include "metadata/metadata_store.hpp"

std::vector<char> handle_request(Reader& reader, MetadataStore& store);
std::vector<char> build_response(int32_t correlation_id, const std::vector<char>& response_data);