#include "api_versions.hpp"
#include "errors.hpp"
#include "protocol/request.hpp"
#include <vector>
#include <cstdint>

std::vector<char> handle_api_versions(RequestHeader& header) {
    int16_t api_key = header.api_key;
    int16_t api_version = header.api_version;
    int32_t correlation_id = header.correlation_id;

    Writer writer;
    if (api_version < 0 || api_version > 4){
        // Handle invalid API version
        writer.write_int32(correlation_id);
        writer.write_int16(static_cast<int16_t>(ErrorCode::UNSUPPORTED_VERSION));
    }
    else {
        // Handle valid API version
        writer.write_int32(correlation_id);
        writer.write_int16(static_cast<int16_t>(ErrorCode::NONE));
    }
    return writer.data();
};