#include "metadata/metadata_store.hpp"
#include "metadata/metadata_log.hpp"
#include <fstream>

std::vector<char> read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return {};
    return std::vector<char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void MetadataStore::load(const std::string& filename) {
    std::vector<char> bytes = read_file(filename);
    topics_ = parse_metadata_log(bytes);
}

const Topic* MetadataStore::find_topic(const std::string& topic_name) {
    for (auto& topic : topics_) {
        if (topic.name == topic_name) {
            return &topic;
        }
    }
    return nullptr;
}






