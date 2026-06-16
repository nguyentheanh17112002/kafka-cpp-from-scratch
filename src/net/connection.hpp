#pragma once
#include <vector>
#include "common/byte_buffer.hpp"

class Connection {
    public:
        explicit Connection(int fd);
        ~Connection();
        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;
        Reader read_request();
        void send(const std::vector<char>& response);
    private:
        int fd_;
        bool read_exact(void* buffer, size_t size);
        bool write_exact(const void* buffer, size_t size);
};