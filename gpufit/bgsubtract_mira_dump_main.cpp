#include "mira_BGSubtract.hpp"
#include "mira_constants.h"
#include <chrono>

int main(int argc, char **argv)
{
    if (argc < 2)
    {

        std::cout << "Usage: bgsubtract_mira_data [input_file.dat]" << std::endl;
        return 1;
    }

    FitThreadPool::Create(1, 1);

    const std::string ifname = argv[1];
    std::ifstream ifs(ifname, std::ios::binary);
    ifs.seekg(0, std::ios::end);
    u_int64_t size = ifs.tellg();
    ifs.seekg(0);

    char *buffer = new char[size];
    ifs.read(buffer, size);

    u_int32_t *buf32 = (u_int32_t *)buffer;
    std::cout << "reading data file..." << std::endl;
    auto data = mira::decode_buffer(buf32, size / (u_int64_t)4, mira::kChannelsToProcess);
    auto subtracted_data = mira::subtract_bg(data, {0, 1000});

    std::cout << "writing data to subtracted_output.json..." << std::endl;
    std::ofstream ofs("subtracted_output.json");
    mira::write_bg_subtracted_data_to_json(ofs, subtracted_data);
    ofs.close();
    ifs.close();
    std::cout << "done." << std::endl;

    return 0;
}