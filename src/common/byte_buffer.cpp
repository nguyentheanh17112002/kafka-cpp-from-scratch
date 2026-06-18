#include"byte_buffer.hpp"
#include<cstring>
#include<arpa/inet.h>
#include<stdexcept>

Reader::Reader(std::vector<char> data){
    data_ = std::move(data);
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