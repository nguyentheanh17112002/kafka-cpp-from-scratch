#pragma once
#include<vector>
#include<cstdint>
#include<cstddef>
#include<string>


class Reader{
    private:
        std::vector<char> data_;
        size_t offset_ = 0;
    public:
        explicit Reader(std::vector<char> data);
        int8_t read_int8();
        int16_t read_int16();
        int32_t read_int32();
        int64_t read_int64();
        int64_t read_signed_varint();
        uint64_t read_unsigned_varint();
        bool has_reamining_data() const;
        size_t position() const;
        std::vector<char> read_bytes(size_t length);
};

class Writer{
    private:
        std::vector<char> data_;
    public:
        void write_int64(int64_t value);
        void write_int32(int32_t value);
        void write_int16(int16_t value);
        void write_int8(int8_t value);
        void write_bytes(const std::vector<char>& bytes);
        void write_unsigned_varint(uint64_t value);
        void write_compact_string(const std::string& str);
        const std::vector<char>& data() const;
};