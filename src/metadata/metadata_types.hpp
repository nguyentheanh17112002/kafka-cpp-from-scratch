#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <array>


struct Partition{
    int32_t partition_index;
    int32_t leader_id;
    int32_t leader_epoch;
    std::vector<int32_t> replica_nodes;
    std::vector<int32_t> isr_nodes;
};

struct Topic{
    std::string name;
    std::array<uint8_t, 16> topic_id;
    std::vector<Partition> partitions;
};

enum class MetadataRecordType : int8_t {
    TOPIC = 2,
    PARTITION = 3
};