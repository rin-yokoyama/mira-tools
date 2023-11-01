#include <iostream>
#include <fstream>
#include "mira_decoder.hpp"

int main(int argc, char **argv)
{
    if (argc < 1)
    {
        std::cout << "Usage: decode_mira_data [input_file.dat]" << std::endl;
        return 1;
    }
    const std::string ifname = argv[1];
    std::ifstream ifs(ifname, std::ios::binary);
    ifs.seekg(0, std::ios::end);
    u_int64_t size = ifs.tellg();
    ifs.seekg(0);

    char *buffer = new char[size];
    ifs.read(buffer, size);

    u_int32_t *buf32 = (u_int32_t *)buffer;
    auto data = mira::decode_buffer(buf32, size / 4, {0});

    std::ofstream ofs("output.json");
    ofs << "[" << std::endl;
    for (const auto &evt : data)
    {
        ofs << "{" << std::endl;
        ofs << "\"event_id\": " << evt.event_id_ << "," << std::endl;
        ofs << "\"ts\": " << evt.ts_ << "," << std::endl;
        ofs << "\"data\": [" << std::endl;
        for (const auto &ch_data : evt.data_)
        {
            ofs << "{\"channel\": " << ch_data.ch_ << "," << std::endl;
            ofs << "\"waveform\": [";
            for (int i = 0; i < ch_data.size_; ++i)
            {
                ofs << ch_data.waveform_[i] << ",";
            }
            ofs.seekp(-1, std::ios_base::cur);
            ofs << "]},\n";
        }
        ofs.seekp(-2, std::ios_base::cur);
        ofs << "\n]},\n";
    }
    ofs.seekp(-2, std::ios_base::cur);
    ofs << "\n]" << std::endl;
    ofs.close();
    ifs.close();

    return 0;
}