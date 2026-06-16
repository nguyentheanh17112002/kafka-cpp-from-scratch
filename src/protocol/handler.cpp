#include "protocol/handler.hpp"

std::vector<char> handle_request(Reader& reader) {
    int16_t api_key = reader.read_int16();
    int16_t api_version = reader.read_int16();
    int32_t correlation_id = reader.read_int32();

    Writer writer;
    writer.write_int32(sizeof(correlation_id));
    writer.write_int32(correlation_id);
    return writer.data();
}