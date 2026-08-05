#include <iostream>
#include "net/tcp_server.hpp"
#include "protocol/handler.hpp"
#include "metadata/metadata_store.hpp"

int main(int argc, char *argv[])
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    MetadataStore store;
    store.load("/tmp/kraft-combined-logs/__cluster_metadata-0/00000000000000000000.log"); //hard code

    try
    {
        TcpServer server(9092);
        server.set_handler(
            [&store](Reader &reader) {
                return handle_request(reader, store);
            }
        );
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
