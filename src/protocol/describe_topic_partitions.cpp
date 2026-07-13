#include "protocol/describe_topic_partitions.hpp"
#include<array>


std::vector<char> handle_describe_topic_partitions(RequestHeader &header, Reader& reader) {
    Writer writer;
    // write response header v1 tag buffer 
    writer.write_unsigned_varint(0); 
    
    // hardcode throttling_time_ms to 0
    writer.write_int32(0); 

    uint64_t num_topics = reader.read_unsigned_varint();
    // write number of topics to response
    writer.write_unsigned_varint(num_topics);

    for (uint64_t i = 0; i < num_topics - 1; ++i) {
        uint64_t topic_name_length = reader.read_unsigned_varint();
        std::string topic_name;
        for (uint64_t j = 0; j < topic_name_length - 1; ++j) {
            topic_name += reader.read_int8();
        }
        // read empty tagged fields
        reader.read_unsigned_varint();

        // write error_code to response (3)
        writer.write_int16(3); // unknown topic or partition

        // write topic_name to response
        writer.write_compact_string(topic_name);

        // write topic_id to response (16 bytes of zeros)
        std::array<uint8_t, 16> topic_id = {0};
        writer.write_bytes(std::vector<char>(topic_id.begin(), topic_id.end()));

        // write is_internal to response (false)
        writer.write_int8(0); // false

        // write partitions array to response (empty)
        writer.write_unsigned_varint(1); // number of partitions: 0

        // write authorized operations
        writer.write_int32(0); 
        
        // write empty tagged fields to response
        writer.write_unsigned_varint(0); // empty tagged fields
    }    
    // write next cursor to response (null)
    writer.write_int8(-1); // null next cursor
    writer.write_unsigned_varint(0); // empty tagged fields
    return writer.data();
}