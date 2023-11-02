#ifndef __MIRA_GPUFIT_HPP__
#define __MIRA_GPUFIT_HPP__

#include "Interfaces/PulseFitInterface.hpp"
#include "mira_decoder.hpp"

namespace mira
{
    struct FitResult
    {
        int index_;
        std::vector<float> params_;
        float chi_squared_;
        int state_;
        int n_iterations_;
    };

    struct OutputData
    {
        mira::FitResult fit_result_;
        int efn_;
        int ch_;
        int event_id_;
        u_int64_t ts_;
    };

    void gpufit_event_data(const std::vector<mira::EventData> &input, std::vector<mira::OutputData> &output, const int batch_size = 10000);

    /**
     * @brief Write output data contents to ofs in json format
     *
     * @param ofs: output file stream
     * @param data: a vector of output data
     */
    void write_output_data_to_json(std::ofstream &ofs, const std::vector<mira::OutputData> &data);
};

#endif /**__MIRA_GPUFIT_HPP__**/
