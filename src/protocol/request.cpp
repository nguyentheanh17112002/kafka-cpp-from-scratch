#include "protocol/request.hpp"

RequestHeader parse_request_header(Reader& reader) {
    RequestHeader header;
    header.api_key = reader.read_int16();
    header.api_version = reader.read_int16();
    header.correlation_id = reader.read_int32();
    int16_t client_id_length = reader.read_int16();
    if (client_id_length > 0) {
        std::vector<char> client_id_bytes(client_id_length);
        for (int i = 0; i < client_id_length; ++i) {
            client_id_bytes[i] = reader.read_int8();
        }
        header.client_id = std::string(client_id_bytes.begin(), client_id_bytes.end());
    } else {
        header.client_id = std::nullopt;
    }
    reader.read_unsigned_varint();
    return header;
}