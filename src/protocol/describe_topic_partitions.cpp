#include "protocol/describe_topic_partitions.hpp"
#include "protocol/errors.hpp"
#include "metadata/metadata_store.hpp"
#include<array>


std::vector<char> handle_describe_topic_partitions(RequestHeader &header, Reader& reader) {
    Writer writer;
    // write response header v1 tag buffer 
    writer.write_unsigned_varint(0); 
    
    // hardcode throttling_time_ms to 0
    writer.write_int32(0); 

    uint64_t num_topics = reader.read_unsigned_varint(); // read number of topics from request
    // write number of topics to response
    writer.write_unsigned_varint(num_topics);

    MetadataStore store;
    store.load("/tmp/kraft-combined-logs/__cluster_metadata-0/00000000000000000000.log"); //hard code

    for (uint64_t i = 0; i < num_topics - 1; ++i) {
        uint64_t topic_name_length = reader.read_unsigned_varint();
        std::string topic_name;
        for (uint64_t j = 0; j < topic_name_length - 1; ++j) {
            topic_name += reader.read_int8();
        }
        // read empty tagged fields
        reader.read_unsigned_varint();
        
        const Topic* topic = store.find_topic(topic_name);
        if (topic) {
            writer.write_int16(static_cast<int16_t>(ErrorCode::NONE));
            writer.write_compact_string(topic->name);
            writer.write_bytes(std::vector<char>(topic->topic_id.begin(), topic->topic_id.end()));
            writer.write_int8(0); // false for is_internal
            writer.write_unsigned_varint(topic->partitions.size() + 1);
            for (const auto& partition : topic->partitions) {
                writer.write_int16(static_cast<int16_t>(ErrorCode::NONE));
                writer.write_int32(partition.partition_index);
                writer.write_int32(partition.leader_id);
                writer.write_int32(partition.leader_epoch);
                writer.write_unsigned_varint(partition.replica_nodes.size() + 1);
                for (const auto& replica : partition.replica_nodes) {
                    writer.write_int32(replica);
                }
                writer.write_unsigned_varint(partition.isr_nodes.size() + 1);
                for (const auto& isr : partition.isr_nodes) {
                    writer.write_int32(isr);
                }
                writer.write_unsigned_varint(1); // eligible_leader_replicas
                writer.write_unsigned_varint(1); // last_known_elr
                writer.write_unsigned_varint(1); // offline_replicas
                writer.write_unsigned_varint(0); // empty tagged fields
            }
            writer.write_int32(0); // authorized operations
            writer.write_unsigned_varint(0); // empty tagged fields 
        }
        else {
            writer.write_int16(static_cast<int16_t>(ErrorCode::UNKNOWN_TOPIC_OR_PARTITION));
            writer.write_compact_string(topic_name);
            std::array<uint8_t, 16> topic_id = {0};
            writer.write_bytes(std::vector<char>(topic_id.begin(), topic_id.end()));
            writer.write_int8(0); // false for is_internal
            writer.write_unsigned_varint(1); // number of partitions: 0
            writer.write_int32(0); // authorized operations
            writer.write_unsigned_varint(0); // empty tagged fields
        }

    }   
    // write next cursor to response (null)
    writer.write_int8(-1); // null next cursor
    writer.write_unsigned_varint(0); // empty tagged fields
    return writer.data();
}