#include "api_versions.hpp"
#include "errors.hpp"
#include "protocol/request.hpp"
#include <vector>
#include <cstdint>

ApiKeyVersion api_versions[] = {
    {18, 0, 4}, // API_VERSIONS
    {0, 0, 2},  // PRODUCE
    {1, 0, 7},  // FETCH
    {3, 0, 3},  // LIST_OFFSETS
    {4, 0, 3}   // METADATA
};

std::vector<char> handle_api_versions(RequestHeader &header)
{
    int16_t api_version = header.api_version;
    Writer writer;
    if (api_version < 0 || api_version > 4)
    {
        writer.write_int16(static_cast<int16_t>(ErrorCode::UNSUPPORTED_VERSION));
    }
    else
    {
        writer.write_int16(static_cast<int16_t>(ErrorCode::NONE));
        writer.write_int8(sizeof(api_versions) / sizeof(ApiKeyVersion) + 1); // length of api_versions array
        for (const auto &api : api_versions)
        {
            writer.write_int16(api.api_key);
            writer.write_int16(api.min_version);
            writer.write_int16(api.max_version);
            writer.write_int8(0); // No tagged fields
        }
        writer.write_int32(0); // throttling_time_ms
        writer.write_int8(0); // No tagged fields
    }
    return writer.data();
};