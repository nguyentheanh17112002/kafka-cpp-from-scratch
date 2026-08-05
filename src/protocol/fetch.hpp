#pragma once
#include <vector>
#include "common/byte_buffer.hpp"
#include "protocol/request.hpp"
#include "metadata/metadata_store.hpp"

std::vector<char> handle_fetch(Reader& reader, MetadataStore& store);