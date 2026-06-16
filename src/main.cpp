#include <iostream>
#include "net/tcp_server.hpp"
#include "protocol/handler.hpp"

int main(int argc, char *argv[])
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    try
    {
        TcpServer server(9092);
        server.set_handler(handle_request);
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
