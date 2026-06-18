#pragma once
#include<vector>
#include<cstdint>
#include<cstddef>


class Reader{
    private:
        std::vector<char> data_;
        size_t offset_ = 0;
    public:
        explicit Reader(std::vector<char> data);
        int16_t read_int16();
        int32_t read_int32();
};

class Writer{
    private:
        std::vector<char> data_;
    public:
        void write_int32(int32_t value);
        void write_int16(int16_t value);
        void write_bytes(const std::vector<char>& bytes);
        const std::vector<char>& data() const;
};