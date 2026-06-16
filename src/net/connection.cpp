#include "net/connection.hpp"
#include "common/byte_buffer.hpp"
#include <unistd.h>
#include <stdexcept>
#include <arpa/inet.h>

Connection::Connection(int fd) : fd_(fd) {};
Connection::~Connection() {
    close(fd_);
};

Reader Connection::read_request() {
    int32_t message_size_network = 0;
    if (!read_exact(&message_size_network, sizeof(message_size_network))) {
        throw std::runtime_error("Failed to read request size");
    }

    int32_t message_size = ntohl(message_size_network);
    if (message_size < 0) {
        throw std::runtime_error("Invalid request size");
    }

    std::vector<char> buffer(message_size);
    if (!read_exact(buffer.data(), buffer.size())) {
        throw std::runtime_error("Failed to read request body");
    }

    return Reader(std::move(buffer));
};

void Connection::send(const std::vector<char>& response) {
    if (!write_exact(response.data(), response.size())) {
        throw std::runtime_error("Failed to send response");
    }
}

bool Connection::read_exact(void* buffer, size_t size) {
    char* buf = static_cast<char*>(buffer);
    size_t total_read = 0;
    while (total_read < size) {
        ssize_t bytes_read = read(fd_, buf + total_read, size - total_read);
        if (bytes_read <= 0) {
            return false; // Error or connection closed
        }
        total_read += bytes_read;
    }
    return true;
};

bool Connection::write_exact(const void* buffer, size_t size) {
    const char* buf = static_cast<const char*>(buffer);
    size_t total_written = 0;
    while (total_written < size) {
        ssize_t bytes_written = write(fd_, buf + total_written, size - total_written);
        if (bytes_written <= 0) {
            return false; // Error
        }
        total_written += bytes_written;
    }
    return true;
};
