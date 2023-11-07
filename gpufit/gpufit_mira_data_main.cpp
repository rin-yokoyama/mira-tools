#include "mira_gpufit.hpp"
#include <chrono>

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
    std::cout << "reading data file..." << std::endl;
    auto data = mira::decode_buffer(buf32, size / 4, {0});

    std::vector<mira::OutputData> output_vec;
    std::cout << "fitting started..." << std::endl;
    std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
    mira::gpufit_event_data(data, output_vec, 10000);
    std::chrono::milliseconds::rep const dt_gpufit = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0).count();
    std::cout << "gpufit in " << static_cast<double>(dt_gpufit) / 1000. << " sec" << std::endl;

    std::cout << "writing data to fit_output.json..." << std::endl;
    std::ofstream ofs("fit_output.json");
    mira::write_output_data_to_json(ofs, output_vec);
    ofs.close();
    ifs.close();
    std::cout << "done." << std::endl;

    return 0;
}