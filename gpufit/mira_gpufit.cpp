#include "mira_gpufit.hpp"

void mira::gpufit_event_data(const std::vector<mira::EventData> &input, std::vector<mira::OutputData> &output, const int batch_size)
{
    if (input.empty())
        return;

    const auto n_samples = input.at(0).data_.at(0).size_;

    PulseFitInterface fitter(batch_size, n_samples, 1);
    fitter.SetPrepulseRange(20);
    fitter.SetInitialRiseTime(1);
    fitter.SetInitialDecayTime(100);
    mira::OutputData result;
    std::vector<int> ch_vec;
    std::vector<int> efn_vec;
    std::vector<int> event_id_vec;
    std::vector<u_int64_t> ts_vec;

    auto gpuFit = [&fitter, &result, &output]()
    {
        fitter.CallGpufit();
        while (!fitter.ReadResults(
            result.fit_result_.index_,
            result.fit_result_.params_,
            result.fit_result_.state_,
            result.fit_result_.chi_squared_,
            result.fit_result_.n_iterations_))
        {
            output.emplace_back(result);
        }
    };

    for (const auto &evt : input)
    {
        for (const auto &ch_data : evt.data_)
        {
            std::vector<float> pulse;
            for (int i = 0; i < ch_data.size_; ++i)
            {
                pulse.emplace_back((float)ch_data.waveform_[i]);
            }
            ch_vec.emplace_back(ch_data.ch_);
            efn_vec.emplace_back(ch_data.efn_);
            event_id_vec.emplace_back(evt.event_id_);
            ts_vec.emplace_back(evt.ts_);
            if (fitter.AddPulse(pulse))
            {
                gpuFit();
            }
        }
    }
    gpuFit();
    for (int i = 0; i < output.size(); ++i)
    {
        output[i].ch_ = ch_vec.at(i);
        output[i].efn_ = efn_vec.at(i);
        output[i].event_id_ = event_id_vec.at(i);
        output[i].ts_ = ts_vec.at(i);
    }

    return;
}

void mira::write_output_data_to_json(std::ofstream &ofs, const std::vector<mira::OutputData> &data)
{
    for (const auto &evt : data)
    {
        ofs << "{" << std::endl;
        ofs << "\"event_id\": " << evt.event_id_ << "," << std::endl;
        ofs << "\"ts\": " << evt.ts_ << "," << std::endl;
        ofs << "\"channel\": " << evt.ch_ << "," << std::endl;
        ofs << "\"efn\": " << evt.efn_ << "," << std::endl;
        ofs << "\"chi_squared\": " << evt.fit_result_.chi_squared_ << "," << std::endl;
        ofs << "\"state\": " << evt.fit_result_.state_ << "," << std::endl;
        ofs << "\"n_iterations\": " << evt.fit_result_.n_iterations_ << "," << std::endl;
        ofs << "\"fit_params\": [";
        for (const auto &param : evt.fit_result_.params_)
        {
            ofs << param << ",";
        }
        ofs.seekp(-1, std::ios_base::cur);
        ofs << "]\n},\n";
    }
    ofs.seekp(-2, std::ios_base::cur);
}