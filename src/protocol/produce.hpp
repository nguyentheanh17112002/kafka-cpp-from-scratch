#pragma once
#include <vector>
#include "common/byte_buffer.hpp"
#include "metadata/metadata_store.hpp"

struct ProducePartition {
    int32_t partition_index;
    std::vector<char> record_batch;
};
struct ProduceTopic {
    std::string topic_name;
    std::vector<ProducePartition> partitions;
};

std::vector<char> handle_produce(Reader& reader, MetadataStore& store);