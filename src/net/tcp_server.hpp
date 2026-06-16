#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include "common/byte_buffer.hpp"

class TcpServer {
    public:
        explicit TcpServer(uint16_t port);
        ~TcpServer();
        TcpServer(const TcpServer&) = delete;
        TcpServer& operator=(const TcpServer&) = delete;
        void set_handler(std::function<std::vector<char>(Reader&)> handler);
        void run();
    private:
        int server_fd_;
        std::function<std::vector<char>(Reader&)> handler_;
};