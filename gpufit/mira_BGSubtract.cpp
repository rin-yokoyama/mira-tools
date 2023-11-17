#include "mira_BGSubtract.hpp"

std::vector<mira::BGSubtractedData> mira::subtract_bg(std::vector<mira::EventData> &input, const std::pair<int, int> &range)
{
    std::vector<BGSubtractedData> output;
    if (input.empty())
        return output;

    int n_fits = 0;
    for (const auto &evt : input)
    {
        n_fits = n_fits + evt.data_.size();
    }

    const auto n_samples = input.at(0).data_.at(0).size_;

    std::vector<int> ch_vec;
    std::vector<int> efn_vec;
    std::vector<int> event_id_vec;
    std::vector<u_int64_t> ts_vec;

    BGFitInterface fitter(n_fits, n_samples);
    fitter.SetFitRange({range.first, range.second});
    fitter.SetInitialCycle(mira::kInitialBGCycle);
    fitter.SetParametersToFit({1, 1, 0, 0, 1});

    for (const auto &evt : input)
    {
        for (const auto &ch_data : evt.data_)
        {
            BGSubtractedData subtracted_data;
            for (int i = 0; i < ch_data.size_; ++i)
            {
                subtracted_data.pulse.emplace_back((float)ch_data.waveform_[i]);
            }
            subtracted_data.fit.ch_ = ch_data.ch_;
            subtracted_data.fit.efn_ = ch_data.efn_;
            subtracted_data.fit.event_id_ = evt.event_id_;
            subtracted_data.fit.ts_ = evt.ts_;
            fitter.AddPulse(subtracted_data.pulse, ch_data.ch_);
            output.emplace_back(subtracted_data);
        }
    }

    fitter.PoolFit();

    {
        int index = 0;
        while (!fitter.ReadResults(
            output[index].fit.fit_result_.index_,
            output[index].fit.fit_result_.params_,
            output[index].fit.fit_result_.init_params_,
            output[index].fit.fit_result_.state_,
            output[index].fit.fit_result_.chi_squared_,
            output[index].fit.fit_result_.n_iterations_))
        {
            ++index;
        }
    }

    fitter.Clear();
    fitter.SetParametersToFit({1, 1, 1, 0, 1});
    for (auto &data : output)
    {
        fitter.AddPulse(data.pulse, data.fit.ch_);
        for (int i = 0; i < fitter.GetNParameters(); ++i)
        {
            fitter.SetInitialParameter(data.fit.fit_result_.index_, i, data.fit.fit_result_.params_[i]);
        }
    }

    fitter.PoolFit();
    {
        int index = 0;
        while (!fitter.ReadResults(
            output[index].fit.fit_result_.index_,
            output[index].fit.fit_result_.params_,
            output[index].fit.fit_result_.init_params_,
            output[index].fit.fit_result_.state_,
            output[index].fit.fit_result_.chi_squared_,
            output[index].fit.fit_result_.n_iterations_))
        {
            fitter.SubtractBackground(output[index].pulse, output[index].fit.fit_result_.params_);
            ++index;
        }
    }

    return output;
}

void mira::write_bg_subtracted_data_to_json(std::ofstream &ofs, const std::vector<mira::BGSubtractedData> &data)
{
    ofs << "[" << std::endl;
    for (const auto &evt : data)
    {
        ofs << "{" << std::endl;
        ofs << "\"event_id\": " << evt.fit.event_id_ << "," << std::endl;
        ofs << "\"ts\": " << evt.fit.ts_ << "," << std::endl;
        ofs << "\"channel\": " << evt.fit.ch_ << "," << std::endl;
        ofs << "\"efn\": " << evt.fit.efn_ << "," << std::endl;
        ofs << "\"chi_squared\": " << evt.fit.fit_result_.chi_squared_ << "," << std::endl;
        ofs << "\"state\": " << evt.fit.fit_result_.state_ << "," << std::endl;
        ofs << "\"n_iterations\": " << evt.fit.fit_result_.n_iterations_ << "," << std::endl;
        ofs << "\"waveform\": [";
        for (const auto &sample : evt.pulse)
        {
            ofs << sample << ",";
        }
        ofs.seekp(-1, std::ios_base::cur);
        ofs << "],\n";
        const auto p = evt.fit.fit_result_.params_;
        ofs << "\"function\": [";
        for (int i = 0; i < evt.pulse.size(); ++i)
        {
            ofs << p[0] * sin(((float)i - p[1]) / p[2]) + p[3] * (float)i + p[4] << ",";
        }
        ofs.seekp(-1, std::ios_base::cur);
        ofs << "],\n";
        ofs << "\"fit_params\": [";
        for (const auto &param : evt.fit.fit_result_.params_)
        {
            ofs << param << ",";
        }
        ofs.seekp(-1, std::ios_base::cur);
        ofs << "],\n";
        ofs << "\"init_params\": [";
        for (const auto &param : evt.fit.fit_result_.init_params_)
        {
            ofs << param << ",";
        }
        ofs.seekp(-1, std::ios_base::cur);
        ofs << "]\n},\n";
    }
    ofs.seekp(-2, std::ios_base::cur);
    ofs << "\n]" << std::endl;
}