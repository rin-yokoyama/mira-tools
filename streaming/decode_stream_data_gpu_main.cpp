#include <iostream>
#include <fstream>
#include <sstream>
#include "mira_decoder.hpp"
#include "mira_streaming.hpp"
#include "mira_ParquetGPU.hpp"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: stream_fitter [bootstrap.servers] [batch_size(=1)] [client.id (= 0)]" << std::endl;
        return 1;
    }
    std::string bootstrap_servers = argv[1];
    int batch_size = 1;
    std::string client_id = "0";
    if (argc > 1)
    {
        batch_size = std::stoi(argv[2]);
    }
    if (argc > 2)
    {
        client_id = argv[2];
    }

    auto topic = mira::init_producer(client_id, bootstrap_servers, "fitdata");
    mira::FitDataWriter writer;
    std::vector<mira::EventData> data_vec;
    int batch_count = 0;
    char longbuff[20824 * 100];
    size_t lb_size = 0;

    auto gpufit = [&writer, &topic, &batch_count, &batch_size, &longbuff, &lb_size](size_t len, void *buff)
    {
        ++batch_count;
        std::memcpy(&longbuff[lb_size], buff, len);
        lb_size += len;
        if (batch_count < batch_size)
            return;
        const u_int64_t size = lb_size;
        const auto buf32 = (u_int32_t *)longbuff;
        auto data = mira::decode_buffer(buf32, size / 4, mira::kChannelsToProcess);
        std::vector<mira::OutputData> output_vec;
        mira::gpufit_multithread(data, output_vec, mira::kNThreads, mira::kNFitAtOnce, mira::kNGpu + mira::kNCpu, mira::kNGpu);

        std::cout << "output size = " << output_vec.size() << std::endl;

        auto table = writer.GenerateTable(output_vec);
        auto stream = writer.WriteStream(table);
        if (stream->size() > 0)
            mira::produce(topic, stream->size(), (void *)stream->data());
        lb_size = 0;
        batch_count = 0;
    };

    mira::polling_loop(client_id, bootstrap_servers, "fitter", "rawdata", gpufit);

    return 0;
}
