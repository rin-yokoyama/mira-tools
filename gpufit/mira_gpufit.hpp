#ifndef __MIRA_GPUFIT_HPP__
#define __MIRA_GPUFIT_HPP__

#include "Interfaces/PulseFitInterface.hpp"
#include "mira_decoder.hpp"
#include <thread>
#include <future>
namespace mira
{
    struct FitResult
    {
        int index_;
        std::vector<float> params_;
        std::vector<float> init_params_;
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
    class FitterClass
    {
    public:
        FitterClass() {}
        virtual ~FitterClass() {}

        std::vector<mira::OutputData> Fit();
        void Clear();
        void Insert(const std::vector<mira::OutputData> &output);

        int id_;
        bool gpu_;
        std::vector<mira::OutputData> *global_output_;
        std::vector<mira::OutputData> output_;
        PulseFitInterface *fitter;
        std::vector<int> ch_vec;
        std::vector<int> efn_vec;
        std::vector<int> event_id_vec;
        std::vector<u_int64_t> ts_vec;
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
