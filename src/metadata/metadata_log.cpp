#include "metadata/metadata_log.hpp"
#include "common/byte_buffer.hpp"
#include <cstdint>
#include <algorithm>

static std::vector<int32_t> read_compact_int32_array(Reader& reader){
    uint64_t array_length = reader.read_unsigned_varint() - 1; // read array length
    std::vector<int32_t> result;
    for (uint64_t i = 0; i < array_length; ++i) {
        result.push_back(reader.read_int32());
    }
    return result;
}

std::vector<Topic> parse_metadata_log(const std::vector<char>& bytes) {
    Reader reader(bytes);
    std::vector<Topic> topics;
    std::vector<std::pair<std::array<uint8_t, 16>, Partition>> pending_partitions;

    while(reader.has_reamining_data()) {
        int64_t base_offset = reader.read_int64();
        int32_t batch_length = reader.read_int32();
        int32_t partition_leader_epoch = reader.read_int32();
        int8_t magic = reader.read_int8();
        int32_t crc = reader.read_int32();
        int16_t attributes = reader.read_int16();
        int32_t last_offset_delta = reader.read_int32();
        int64_t base_timestamp = reader.read_int64();
        int64_t max_timestamp = reader.read_int64();
        int64_t producer_id = reader.read_int64();
        int16_t producer_epoch = reader.read_int16();
        int32_t base_sequence = reader.read_int32();

        int32_t records_count = reader.read_int32();

        for (int i = 0; i < records_count; ++i) {
            reader.read_signed_varint(); // length
            reader.read_int8(); // attributes
            reader.read_signed_varint(); // timestamp_delta
            reader.read_signed_varint(); // offset_delta

            int64_t key_length = reader.read_signed_varint();
            if (key_length > 0) {
                reader.read_bytes(static_cast<size_t>(key_length)); // key
            }
            
            int64_t value_length = reader.read_signed_varint();
            size_t value_start = reader.position(); 

            reader.read_int8(); // frame version
            int8_t type = reader.read_int8(); // type
            reader.read_int8(); //version 

            switch (static_cast<MetadataRecordType>(type)){
                case MetadataRecordType::TOPIC:
                {
                    uint64_t topic_name_length = reader.read_unsigned_varint() - 1;
                    std::string topic_name;
                    std::vector<char> topic_name_bytes = reader.read_bytes(static_cast<size_t>(topic_name_length));
                    topic_name = std::string(topic_name_bytes.begin(), topic_name_bytes.end());
                    std::array<uint8_t, 16> topic_id;
                    for (size_t j = 0; j < topic_id.size(); ++j) {
                        topic_id[j] = static_cast<uint8_t>(reader.read_int8());
                    }
                    Topic topic{topic_name, topic_id, {}};
                    topics.push_back(topic);
                    break;
                }
                case MetadataRecordType::PARTITION:
                {
                    int32_t partition_index = reader.read_int32();
                    std::vector<char> tid_bytes = reader.read_bytes(16); // topic id
                    std::array<uint8_t, 16> tid;
                    std::copy(tid_bytes.begin(), tid_bytes.end(), tid.begin());

                    std::vector<int32_t> replica_nodes = read_compact_int32_array(reader);
                    std::vector<int32_t> isr_nodes = read_compact_int32_array(reader);
                    
                    read_compact_int32_array(reader); 
                    read_compact_int32_array(reader);

                    int32_t leader_id = reader.read_int32();
                    int32_t leader_epoch = reader.read_int32();
                    reader.read_int32(); // partition_epoch

                    uint64_t dir_count = reader.read_unsigned_varint();
                    for (uint64_t j = 0; j + 1 < dir_count; ++j){
                        reader.read_bytes(16); // directory
                    }

                    reader.read_unsigned_varint(); // tagged fields

                    Partition partition{partition_index, leader_id, leader_epoch, replica_nodes, isr_nodes};
                    pending_partitions.emplace_back(tid, partition);
                    break;
                }
                default:
                {
                    break;
                }
            }
            size_t consumed = reader.position() - value_start;
            reader.read_bytes(static_cast<size_t>(value_length) - consumed); // skip remaining bytes in value
            reader.read_unsigned_varint(); // headers_count
        }
    }
    for (const auto& [tid, partition] : pending_partitions) {
        auto it = std::find_if(topics.begin(), topics.end(), [&tid](const Topic& topic) {
            return topic.topic_id == tid;
        });
        if (it != topics.end()) {
            it->partitions.push_back(partition);
        }
    }
    pending_partitions.clear();
    return topics;
}
