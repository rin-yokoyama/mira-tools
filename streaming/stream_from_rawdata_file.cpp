#include <iostream>
#include <string>
#include <fstream>
#include "mira_decoder.hpp"
#include "mira_streaming.hpp"
#include "mira_Parquet.hpp"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: file2stream [input_file.dat] [bootstrap.servers] [client.id (=0)]" << std::endl;
        return 1;
    }
    const std::string ifname = argv[1];
    const std::string bootstrap_servers = argv[2];
    std::string client_id("0");
    if (argc > 3)
    {
        client_id = argv[3];
    }

    auto topic = mira::init_producer(client_id, bootstrap_servers, "decoded");

    const int kNEventsPerBuff = 1;
    std::ifstream ifs(ifname, std::ios::binary);
    ifs.seekg(0, std::ios::end);
    auto size = ifs.tellg();
    ifs.seekg(0);
    auto cpos = ifs.tellg();
    int nevents = 0;
    while (cpos < size - 1)
    {
        char buff[4];
        ifs.read(buff, 4);
        u_int32_t *header = (u_int32_t *)buff;
        auto event_size = mira::get_size32(header[0]);
        ifs.seekg((event_size - 1) * 4, std::ios::cur);
        auto rpos = ifs.tellg();
        ++nevents;
        if (nevents >= kNEventsPerBuff)
        {
            ifs.seekg(cpos);
            size_t buffsize = rpos - cpos;
            char *buffer = new char[buffsize];
            ifs.read(buffer, buffsize);
            u_int32_t *buf32 = (u_int32_t *)buffer;
            auto data = mira::decode_buffer(buf32, buffsize / 4, mira::kChannelsToProcess);
            auto json_data = mira::event_data_to_json(data);
            // std::cout << json_data << std::endl;

            mira::produce(topic, sizeof(json_data.c_str()), (void *)json_data.c_str());
            nevents = 0;
            cpos = ifs.tellg();
            delete[] buffer;
        }
    }

    return 0;
}