#include "protocol/handler.hpp"
#include "protocol/api_versions.hpp"
#include "protocol/request.hpp"

std::vector<char> build_response(int32_t correlation_id, const std::vector<char> &response_data)
{
    Writer writer;
    writer.write_int32(response_data.size());
    writer.write_bytes(response_data);
    return writer.data();
}

std::vector<char> handle_request(Reader &reader)
{
    RequestHeader header = parse_request_header(reader);
    std::vector<char> response_data = handle_api_versions(header);
    return build_response(header.correlation_id, response_data);
}
