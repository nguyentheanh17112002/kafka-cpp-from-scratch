#pragma once 
#include <vector>
#include <string>
#include "metadata/metadata_types.hpp"

std::vector<char> read_file(const std::string& filename);
void append_file(const std::string& filename, const std::vector<char>& data);
class MetadataStore {
    public:
        void load(const std::string& filename);
        const Topic* find_topic(const std::string& topic_name);
        const Topic* find_topic_by_id(const std::array<uint8_t, 16>& topic_id);
    private:
        std::vector<Topic> topics_;
};
