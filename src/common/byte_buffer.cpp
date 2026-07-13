#include"byte_buffer.hpp"
#include<cstring>
#include<arpa/inet.h>
#include<stdexcept>

Reader::Reader(std::vector<char> data){
    data_ = std::move(data);
}

int8_t Reader::read_int8(){
    if(offset_ + sizeof(int8_t) > data_.size()){
        throw std::out_of_range("Not enough data to read int8");
    }
    int8_t value = static_cast<int8_t>(data_[offset_]);
    offset_ += sizeof(int8_t);
    return value;
}

int16_t Reader::read_int16(){
    if(offset_ + sizeof(int16_t) > data_.size()){
        throw std::out_of_range("Not enough data to read int16");
    }
    int16_t value;
    std::memcpy(&value, &data_[offset_], sizeof(int16_t));
    offset_ += sizeof(int16_t);
    return ntohs(value);
}

int32_t Reader::read_int32(){
    if(offset_ + sizeof(int32_t) > data_.size()){
        throw std::out_of_range("Not enough data to read int32");
    }
    int32_t value;
    std::memcpy(&value, &data_[offset_], sizeof(int32_t));
    offset_ += sizeof(int32_t);
    return ntohl(value);
}

uint64_t Reader::read_unsigned_varint() {
    uint64_t value = 0;
    int shift = 0;
    while (true) {
        if (offset_ >= data_.size()) {
            throw std::out_of_range("Not enough data to read unsigned varint");
        }
        uint8_t byte = static_cast<uint8_t>(data_[offset_++]);
        value |= static_cast<uint64_t>(byte & 0x7F) << shift;
        if ((byte & 0x80) == 0) {
            break;
        }
        shift += 7;
        if (shift >= 64) {
            throw std::overflow_error("Unsigned varint is too large");
        }
    }
    return value;
}

const std::vector<char>& Writer::data() const{
    return data_;
}

void Writer::write_int32(int32_t value){
    int32_t network_value = htonl(value);
    char bytes[sizeof(int32_t)];
    std::memcpy(bytes, &network_value, sizeof(int32_t));
    data_.insert(data_.end(), bytes, bytes + sizeof(int32_t));
}

void Writer::write_int16(int16_t value){
    int16_t network_value = htons(value);
    char bytes[sizeof(int16_t)];
    std::memcpy(bytes, &network_value, sizeof(int16_t));
    data_.insert(data_.end(), bytes, bytes + sizeof(int16_t));
}

void Writer::write_int8(int8_t value){
    data_.push_back(static_cast<char>(value));
}

void Writer::write_bytes(const std::vector<char>& bytes){
    data_.insert(data_.end(), bytes.begin(), bytes.end());
}

void Writer::write_unsigned_varint(uint64_t value) {
    while (value > 0x7F) {
        data_.push_back(static_cast<char>((value & 0x7F) | 0x80));
        value >>= 7;
    }
    data_.push_back(static_cast<char>(value));
}

void Writer::write_compact_string(const std::string& str) {
    write_unsigned_varint(str.size() + 1);
    data_.insert(data_.end(), str.begin(), str.end());
}