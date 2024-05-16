#include <iostream>
#include <string>
#include <fstream>
#include "mira_constants.h"
#include "mira_decoder.hpp"
#include "mira_streaming.hpp"
#include "mira_ParquetGPU.hpp"
#include "mira_gpufit.hpp"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: file2stream [input_file.dat] [bootstrap.servers] [nevents_per_buff(=100)] [client.id (=0)]" << std::endl;
        return 1;
    }
    const std::string ifname = argv[1];
    const std::string bootstrap_servers = argv[2];
    std::string client_id("0");
    int neve = 100;
    if (argc > 3)
    {
        neve = std::stoi(argv[3]);
    }
    if (argc > 4)
    {
        client_id = argv[4];
    }

    auto topic = mira::init_producer(client_id, bootstrap_servers, "decoded");
    mira::RawDataWriter writer;

    const int kNEventsPerBuff = neve;
    std::ifstream ifs(ifname, std::ios::binary);
    ifs.seekg(0, std::ios::end);
    auto size = ifs.tellg();
    ifs.seekg(0);
    auto cpos = ifs.tellg();
    int nevents = 0;
    while (cpos < (size - std::streampos(1)))
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

            std::vector<mira::OutputData> output_vec;
            mira::gpufit_multithread(data, output_vec, mira::kNThreads, mira::kNFitAtOnce, mira::kNGpu + mira::kNCpu, mira::kNGpu);

            auto table = writer.GenerateTable(data);
            auto stream = writer.WriteStream(table);
            mira::produce(topic, stream->size(), (void *)stream->data());

            nevents = 0;
            cpos = ifs.tellg();
            delete[] buffer;
        }
    }

    return 0;
}