#ifndef __MIRA_GPUFIT_HPP__
#define __MIRA_GPUFIT_HPP__

#include <thread>
#include <future>
#include "mira_FitterClass.hpp"
#include "Interfaces/FitThreadPool.hpp"

namespace mira
{

    void gpufit_multithread(
        const std::vector<mira::EventData> &input,
        std::vector<mira::OutputData> &output,
        const int n_threads = 1,
        const int batch_size = 10000,
        const int pool_size = 1,
        const int n_gpu = 1);

    // void gpufit_event_data(const std::vector<mira::EventData> &input, std::vector<mira::OutputData> &output, const int batch_size = 10000);

    // void gpufit_single_batch(const std::vector<mira::EventData> &input, std::vector<mira::OutputData> &output);

    /**
     * @brief Write output data contents to ofs in json format
     *
     * @param ofs: output file stream
     * @param data: a vector of output data
     */
    void write_output_data_to_json(std::ofstream &ofs, const std::vector<mira::OutputData> &data);
};

#endif /**__MIRA_GPUFIT_HPP__**/
