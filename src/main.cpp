#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <netdb.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

bool read_exact(int fd, void* buffer, size_t size) {
    char* cursor = static_cast<char*>(buffer);
    size_t total_read = 0;

    while (total_read < size) {
        ssize_t bytes_read = read(fd, cursor + total_read, size - total_read);
        if (bytes_read <= 0) {
            return false;
        }

        total_read += bytes_read;
    }

    return true;
}

bool write_exact(int fd, const void* buffer, size_t size) {
    const char* cursor = static_cast<const char*>(buffer);
    size_t total_written = 0;

    while (total_written < size) {
        ssize_t bytes_written = write(fd, cursor + total_written, size - total_written);
        if (bytes_written <= 0) {
            return false;
        }

        total_written += bytes_written;
    }

    return true;
}

int16_t read_int16(const std::vector<char>& buffer, size_t& offset) {
    int16_t network_value = 0;
    std::memcpy(&network_value, buffer.data() + offset, sizeof(network_value));
    offset += sizeof(network_value);
    return ntohs(network_value);
}

int32_t read_int32(const std::vector<char>& buffer, size_t& offset) {
    int32_t network_value = 0;
    std::memcpy(&network_value, buffer.data() + offset, sizeof(network_value));
    offset += sizeof(network_value);
    return ntohl(network_value);
}

int main(int argc, char* argv[]) {
    // Disable output buffering
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // get the server socket file descriptor
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create server socket: " << std::endl;
        return 1;
    }

    // Since the tester restarts your program quite often, setting SO_REUSEADDR
    // ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        close(server_fd);
        std::cerr << "setsockopt failed: " << std::endl;
        return 1;
    }

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9092);

    if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) != 0) {
        close(server_fd);
        std::cerr << "Failed to bind to port 9092" << std::endl;
        return 1;
    }

    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        close(server_fd);
        std::cerr << "listen failed" << std::endl;
        return 1;
    }

    std::cout << "Waiting for a client to connect...\n";

    struct sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cerr << "Logs from your program will appear here!\n";
    
    // TODO: Uncomment the code below to pass the first stage
    // 
    int client_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);
    std::cout << "Client connected\n";

    int32_t message_size_network = 0;
    if (!read_exact(client_fd, &message_size_network, sizeof(message_size_network))) {
        std::cerr << "Failed to read message size\n";
        close(client_fd);
        close(server_fd);
        return 1;
    }

    int32_t message_size = ntohl(message_size_network);
    std::vector<char> request(message_size);
    if (!read_exact(client_fd, request.data(), request.size())) {
        std::cerr << "Failed to read request\n";
        close(client_fd);
        close(server_fd);
        return 1;
    }

    size_t offset = 0;
    int16_t api_key = read_int16(request, offset);
    int16_t api_version = read_int16(request, offset);
    int32_t correlation_id = read_int32(request, offset);

    std::cerr << "api_key=" << api_key << "\n";
    std::cerr << "api_version=" << api_version << "\n";
    std::cerr << "correlation_id=" << correlation_id << "\n";

    int32_t response_size_network = htonl(sizeof(correlation_id));
    int32_t correlation_id_network = htonl(correlation_id);

    if (!write_exact(client_fd, &response_size_network, sizeof(response_size_network)) ||
        !write_exact(client_fd, &correlation_id_network, sizeof(correlation_id_network))) {
        std::cerr << "Failed to write response\n";
        close(client_fd);
        close(server_fd);
        return 1;
    }

    close(client_fd);

    close(server_fd);
    return 0;
}
