#pragma once
#include <string>
#include <vector>
#include "protocol/request.hpp"
#include "common/byte_buffer.hpp"
#include "metadata/metadata_store.hpp"

std::vector<char> handle_describe_topic_partitions(Reader& reader, MetadataStore& store);