#include "protocol/handler.hpp"
#include "protocol/api_versions.hpp"
#include "protocol/request.hpp"
#include "protocol/api_keys.hpp"
#include "protocol/describe_topic_partitions.hpp"

std::vector<char> build_response(int32_t correlation_id, const std::vector<char> &response_data)
{
    Writer writer;
    writer.write_int32(static_cast<int32_t>(sizeof(correlation_id) + static_cast<int32_t>(response_data.size())));
    writer.write_int32(correlation_id);
    writer.write_bytes(response_data);
    return writer.data();
}

std::vector<char> handle_request(Reader &reader)
{
    RequestHeader header = parse_request_header(reader);
    std::vector<char> response_data;

    switch (header.api_key) {
        case static_cast<int16_t>(APIKey::API_VERSIONS):
            response_data = handle_api_versions(header);
            break;
        case static_cast<int16_t>(APIKey::DESCRIBE_TOPIC_PARTITIONS):
            response_data = handle_describe_topic_partitions(header, reader);
            break;
        default:
            break;
    }

    return build_response(header.correlation_id, response_data);
}
