#pragma once 
#include <vector>
#include <string>
#include "metadata/metadata_types.hpp"

std::vector<char> read_file(const std::string& filename);

class MetadataStore {
    public:
        void load(const std::string& filename);
        const Topic* find_topic(const std::string& topic_name);
    private:
        std::vector<Topic> topics_;
};
