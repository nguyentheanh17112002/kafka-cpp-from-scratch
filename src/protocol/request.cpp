#include "protocol/request.hpp"

RequestHeader parse_request_header(Reader& reader) {
    RequestHeader header;
    header.api_key = reader.read_int16();
    header.api_version = reader.read_int16();
    header.correlation_id = reader.read_int32();
    return header;
}