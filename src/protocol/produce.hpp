#pragma once
#include <vector>
#include "common/byte_buffer.hpp"
#include "metadata/metadata_store.hpp"

std::vector<char> handle_produce(Reader& reader, MetadataStore& store);