#include <iostream>
#include <fstream>
#include <sstream>
#include "mira_decoder.hpp"
#include "mira_streaming.hpp"
#include "mira_Parquet.hpp"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: stream_decoder [bootstrap.servers] [client.id (= 0)]" << std::endl;
        return 1;
    }
    std::string bootstrap_servers = argv[1];
    std::string client_id = "0";
    if (argc > 1)
    {
        client_id = argv[2];
    }

    auto topic = mira::init_producer(client_id, bootstrap_servers, "decoded");
    mira::RawDataWriter writer;

    auto decode = [&writer, &topic](size_t len, void *buff)
    {
        const u_int64_t size = len;
        const auto buf32 = (u_int32_t *)buff;
        auto data = mira::decode_buffer(buf32, size / 4, mira::kChannelsToProcess);

        // auto json_data = mira::event_data_to_json(data);
        // std::cout << json_data << std::endl;

        auto table = writer.GenerateTable(data);
        auto stream = writer.WriteStream(table);
        mira::produce(topic, stream->size(), (void *)stream->data());
    };

    mira::polling_loop(client_id, bootstrap_servers, "decode", "rawdata", decode);

    return 0;
}
