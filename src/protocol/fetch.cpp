#include "protocol/fetch.hpp"
#include "protocol/errors.hpp"
#include <vector>
#include <array>

struct FetchTopic {
    std::array<uint8_t, 16> topic_id;
    std::vector<int32_t> partitions;
};

std::vector<char> handle_fetch(Reader& reader) {
    reader.read_int32(); // max wait ms
    reader.read_int32(); // min bytes
    reader.read_int32(); // max bytes
    reader.read_int8(); // isolation level
    reader.read_int32(); // session id
    reader.read_int32(); // session epoch

    uint64_t num_topics = reader.read_unsigned_varint() - 1; // read number of topics from request
    std::vector<FetchTopic> fetch_topics;
    for (uint64_t i = 0; i < num_topics; ++i){
        std::array<uint8_t, 16> topic_id;
        for (int j = 0; j < 16; ++j) {
            topic_id[j] = static_cast<uint8_t>(reader.read_int8());
        }
        uint64_t num_partitions = reader.read_unsigned_varint() - 1; // read number of partitions for this topic
        std::vector<int32_t> partitions;
        for (uint64_t j = 0; j < num_partitions; ++j) {
            int32_t partition_index = reader.read_int32();
            partitions.push_back(partition_index);
            reader.read_int32(); // current leader epoch
            reader.read_int64(); // fetch offset
            reader.read_int32(); // last fetched epoch
            reader.read_int64(); // log start offset
            reader.read_int32(); // partition max bytes
            reader.read_unsigned_varint(); // tag buffer
        }
        reader.read_unsigned_varint(); // tag buffer for topic
        fetch_topics.push_back({topic_id, partitions});
    }


    Writer writer;

    writer.write_unsigned_varint(0); // respone header tag buffer
    writer.write_int32(0); // throttle time ms
    writer.write_int16(static_cast<int16_t>(ErrorCode::NONE)); // error code
    writer.write_int32(0); // session id
    writer.write_unsigned_varint(fetch_topics.size() + 1); // number of topics in response  
    
    for (const auto& fetch_topic : fetch_topics){
        writer.write_bytes(std::vector<char>(fetch_topic.topic_id.begin(), fetch_topic.topic_id.end()));
        writer.write_unsigned_varint(fetch_topic.partitions.size() + 1); 
        for (const auto& partition_index : fetch_topic.partitions){
            writer.write_int32(partition_index);
            writer.write_int16(static_cast<int16_t>(ErrorCode::UNKNOWN_TOPIC_ID)); 
            writer.write_int64(0); // high watermark
            writer.write_int64(0); // last stable offset
            writer.write_int64(0); // log start offset
            writer.write_unsigned_varint(1); // aborted transactions
            writer.write_int32(-1); // preferred read replica
            writer.write_unsigned_varint(0); // record: null 
            writer.write_unsigned_varint(0); // partition tag buffer
        }
        writer.write_unsigned_varint(0); // topic tag buffer
    }
    writer.write_unsigned_varint(0); // response tag buffer
    return writer.data();
}