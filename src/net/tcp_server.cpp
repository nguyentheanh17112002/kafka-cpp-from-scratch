#include "net/tcp_server.hpp"
#include "net/connection.hpp"
#include <unistd.h>
#include <stdexcept>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>

TcpServer::TcpServer(uint16_t port)
{
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0)
    {
        throw std::runtime_error("Failed to create socket");
    }
    int reuse = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        close(server_fd_);
        throw std::runtime_error("Failed to set socket options");
    }
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd_, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        close(server_fd_);
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(server_fd_, 5) < 0)
    {
        close(server_fd_);
        throw std::runtime_error("Failed to listen on socket");
    }
};

TcpServer::~TcpServer()
{
    close(server_fd_);
};

void TcpServer::set_handler(std::function<std::vector<char>(Reader &)> handler)
{
    handler_ = std::move(handler);
};

void TcpServer::run()
{
    while (true)
    {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0)
        {
            continue; // Accept failed, try again
        }

        try
        {
            Connection connection(client_fd);
            Reader request = connection.read_request();
            std::vector<char> response = handler_(request);
            connection.send(response);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error handling connection: " << e.what() << std::endl;
        }
    }
};
