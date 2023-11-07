#include "mira_gpufit.hpp"

std::vector<mira::OutputData> mira::FitterClass::Fit()
{
    mira::OutputData result;
    if (gpu_)
        fitter->CallGpufit();
    else
        fitter->CallCpufit();
    while (!fitter->ReadResults(
        result.fit_result_.index_,
        result.fit_result_.params_,
        result.fit_result_.init_params_,
        result.fit_result_.state_,
        result.fit_result_.chi_squared_,
        result.fit_result_.n_iterations_))
    {
        output_.emplace_back(result);
    }
    for (int i = 0; i < output_.size(); ++i)
    {
        output_[i].ch_ = ch_vec.at(i);
        output_[i].efn_ = efn_vec.at(i);
        output_[i].event_id_ = event_id_vec.at(i);
        output_[i].ts_ = ts_vec.at(i);
    }
    return output_;
    // Ready();
}

void mira::FitterClass::Insert(const std::vector<mira::OutputData> &output)
{
    global_output_->insert(global_output_->end(), output.begin(), output.end());
    Clear();
}

void mira::FitterClass::Clear()
{
    output_.clear();
    ch_vec.clear();
    efn_vec.clear();
    event_id_vec.clear();
    ts_vec.clear();
}

void mira::gpufit_event_data(const std::vector<mira::EventData> &input, std::vector<mira::OutputData> &output, const int batch_size)
{
    if (input.empty())
        return;

    const auto n_samples = input.at(0).data_.at(0).size_;

    std::vector<mira::FitterClass> fitter_vec(5);
    for (auto &fitter : fitter_vec)
    {
        fitter.fitter = new PulseFitInterface(batch_size, n_samples, 1);
        fitter.fitter->SetPrepulseRange(200);
        // fitter.fitter->SetInitialPeakTime(480);
        fitter.fitter->SetInitialRiseTime(10);
        fitter.fitter->SetInitialDecayTime(100);
        fitter.fitter->SetParametersToFit({1, 1, 1, 1, 1});
        fitter.global_output_ = &output;
        fitter.gpu_ = false;
    }
    for (int i = 0; i < 5; ++i)
    {
        fitter_vec[i].id_ = i;
    }
    fitter_vec[0].gpu_ = 0;
    fitter_vec[2].gpu_ = 1;

    std::vector<bool> first_batch(5, true);
    std::vector<std::future<std::vector<mira::OutputData>>> future_vec(5);

    auto switchFitter = [&fitter_vec, &future_vec, &first_batch]()
    {
        std::rotate(fitter_vec.begin(), fitter_vec.begin() + 1, fitter_vec.end());
        std::rotate(future_vec.begin(), future_vec.begin() + 1, future_vec.end());
        std::rotate(first_batch.begin(), first_batch.begin() + 1, first_batch.end());
        future_vec.back() = std::async(&mira::FitterClass::Fit, fitter_vec.back());
        first_batch.back() = false;
        if (!first_batch[0])
        {
            std::cout << "fitter[" << fitter_vec.back().id_ << "] started. Waiting for fitter[" << fitter_vec[0].id_ << "]" << std::endl;
            fitter_vec[0].Insert(future_vec[0].get());
            first_batch[0] = true;
        }
        std::cout << "fitter[" << fitter_vec[0].id_ << "] is ready" << std::endl;
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
            fitter_vec[0].ch_vec.emplace_back(ch_data.ch_);
            fitter_vec[0].efn_vec.emplace_back(ch_data.efn_);
            fitter_vec[0].event_id_vec.emplace_back(evt.event_id_);
            fitter_vec[0].ts_vec.emplace_back(evt.ts_);
            if (fitter_vec[0].fitter->AddPulse(pulse))
            {
                switchFitter();
            }
        }
    }
    switchFitter();
    for (int i = 0; i < future_vec.size(); ++i)
    {
        if (!first_batch[i])
        {
            std::cout << "Waiting for fitter[" << fitter_vec[i].id_ << "]" << std::endl;
            fitter_vec[i].Insert(future_vec[i].get());
        }
    }
    return;
}

void mira::write_output_data_to_json(std::ofstream &ofs, const std::vector<mira::OutputData> &data)
{
    ofs << "[" << std::endl;
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
        ofs << "],\n";
        ofs << "\"init_params\": [";
        for (const auto &param : evt.fit_result_.init_params_)
        {
            ofs << param << ",";
        }
        ofs.seekp(-1, std::ios_base::cur);
        ofs << "]\n},\n";
    }
    ofs.seekp(-2, std::ios_base::cur);
    ofs << "\n]" << std::endl;
}