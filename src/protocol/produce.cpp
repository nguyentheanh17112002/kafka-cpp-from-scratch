#include "protocol/produce.hpp"
#include "protocol/errors.hpp"
#include "protocol/request.hpp"
#include "common/byte_buffer.hpp"
#include <vector>
#include <algorithm>

static std::vector<ProduceTopic> read_produce_topics(Reader& reader) {
    reader.read_unsigned_varint(); //transactional_id
    reader.read_int16(); //acks
    reader.read_int32(); //timeout_ms
    uint64_t num_topics = reader.read_unsigned_varint() - 1; // read
    std::vector<ProduceTopic> produce_topics;
    for (uint64_t i = 0; i < num_topics; ++i){
        uint64_t topic_name_length = reader.read_unsigned_varint() - 1;
        std::vector<char> topic_name_bytes = reader.read_bytes(static_cast<size_t>(topic_name_length));
        std::string topic_name(topic_name_bytes.begin(), topic_name_bytes.end());
        uint64_t num_partitions = reader.read_unsigned_varint() - 1; // read number of partitions for this topic
        std::vector<ProducePartition> partitions;
        for (uint64_t j = 0; j < num_partitions; ++j) {
            int32_t partition_index = reader.read_int32();
            uint64_t record_batch_length = reader.read_unsigned_varint() - 1; // read record batch length
            std::vector<char> record_batch = reader.read_bytes(static_cast<size_t>(record_batch_length));
            partitions.push_back({partition_index, record_batch}); 
            reader.read_unsigned_varint(); // read tag buffer
        }
        reader.read_unsigned_varint(); // read tag buffer
        produce_topics.push_back({topic_name, partitions});
    }
    reader.read_unsigned_varint(); // read tag buffer
    return produce_topics;
}

std::vector<char> handle_produce(Reader& reader, MetadataStore& store) {
    std::vector<ProduceTopic> produce_topics = read_produce_topics(reader);
    Writer writer;
    writer.write_unsigned_varint(0); // respone header tag buffer 
    writer.write_unsigned_varint(static_cast<uint64_t>(produce_topics.size() + 1)); // number of topics in the response
    for (const auto& produce_topic : produce_topics) {
        writer.write_compact_string(produce_topic.topic_name);
        writer.write_unsigned_varint(static_cast<uint64_t>(produce_topic.partitions.size() + 1)); // number of partitions in the response
        const Topic* topic = store.find_topic(produce_topic.topic_name);
        for (const auto& produce_partition : produce_topic.partitions) {
            writer.write_int32(produce_partition.partition_index);
            bool partition_found = true;
            if (topic){
                auto it = std::find_if(topic->partitions.begin(), topic->partitions.end(), [&](const Partition& p) {
                    return p.partition_index == produce_partition.partition_index;
                });
                if (it == topic->partitions.end()) {
                    partition_found = false;
                }
            }
            if (!topic || !partition_found) {
                writer.write_int16(static_cast<int16_t>(ErrorCode::UNKNOWN_TOPIC_OR_PARTITION));
                writer.write_int64(-1); // offset
                writer.write_int64(-1); // log append time
                writer.write_int64(-1); // log start offset
                writer.write_unsigned_varint(1); // record errors
                writer.write_unsigned_varint(0); // error message
                writer.write_unsigned_varint(0); // tag buffer
            } else {
                std::string file_path = "/tmp/kraft-combined-logs/" + produce_topic.topic_name + "-" + std::to_string(produce_partition.partition_index) + "/00000000000000000000.log";
                append_file(file_path, produce_partition.record_batch);
                writer.write_int16(static_cast<int16_t>(ErrorCode::NONE));
                writer.write_int64(0); // offset
                writer.write_int64(-1); // log append time
                writer.write_int64(0); // log start offset
                writer.write_unsigned_varint(1); // record errors
                writer.write_unsigned_varint(0); // error message
                writer.write_unsigned_varint(0); // tag buffer
            }
        }
        writer.write_unsigned_varint(0); // tag buffer for topic
    }
    writer.write_int32(0); // throttle time
    writer.write_unsigned_varint(0); // tag buffer
    return writer.data();
}