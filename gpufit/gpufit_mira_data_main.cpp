#include "mira_gpufit.hpp"

int main(int argc, char **argv)
{
    if (argc < 2)
    {

        std::cout << "Usage: gpufit_mira_data [input_file.dat]" << std::endl;
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

    std::vector<mira::OutputData> output_vec;
    mira::gpufit_event_data(data, output_vec, 10000);

    std::ofstream ofs("fit_output.json");
    mira::write_output_data_to_json(ofs, output_vec);
    ofs.close();
    ifs.close();

    return 0;
}