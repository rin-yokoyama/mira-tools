#include <iostream>
#include <fstream>
#include "mira_decoder.hpp"
#include "mira_Parquet.hpp"

int main(int argc, char **argv)
{
    if (argc < 2)
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
    auto data = mira::decode_buffer(buf32, size / 4, {0, 1, 2, 3, 4});

    auto table = mira::generate_table(data);
    mira::wirte_parquet_file("output.parquet", table);
    return 0;
}