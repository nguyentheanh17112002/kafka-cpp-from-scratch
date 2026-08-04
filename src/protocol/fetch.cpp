#include "protocol/fetch.hpp"
#include "protocol/errors.hpp"
#include <vector>

std::vector<char> handle_fetch(RequestHeader &header, Reader& reader) {
    Writer writer;
    writer.write_unsigned_varint(0); // tag buffer 
    writer.write_int32(0); // throttle_time_ms
    writer.write_int16(static_cast<int16_t>(ErrorCode::NONE)); // error_code
    writer.write_int32(0); // session_id
    writer.write_unsigned_varint(1); // compact array 
    writer.write_unsigned_varint(0); // tag buffer
    return writer.data();
}