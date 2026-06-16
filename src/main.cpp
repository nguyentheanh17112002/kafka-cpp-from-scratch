#include <iostream>
#include "common/byte_buffer.hpp"
#include "net/connection.hpp"
#include "net/tcp_server.hpp"

std::vector<char> handle_request(Reader &reader)
{
    int16_t api_key = reader.read_int16();
    int16_t api_version = reader.read_int16();
    int32_t correlation_id = reader.read_int32();

    Writer writer;
    writer.write_int32(sizeof(correlation_id));
    writer.write_int32(correlation_id);
    return writer.data();
}

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
