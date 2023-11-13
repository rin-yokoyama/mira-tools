#include "mira_FitterClass.hpp"

int mira::FitterClass::static_id_ = 0;

void mira::FitterClass::Init(int n_fits, int n_samples)
{
    if (fitter)
    {
        delete fitter;
        fitter = nullptr;
    }
    if (bg_fitter_)
    {
        delete bg_fitter_;
        bg_fitter_ = nullptr;
    }
    n_fits_ = n_fits;
    fitter = new PulseFitInterface(n_fits, n_samples, mira::kChPolarityMap);
    const int bg_size = bg_range_.second - bg_range_.first;
    if (bg_range_.second > 0 && bg_size > 0)
    {
        bg_fitter_ = new BGFitInterface(n_fits, bg_size);
    }
    fitter->SetFitRange(mira::kPulseFitRange);
    fitter->SetPrepulseRange(mira::kPrePulseRange);
    if (mira::kInitialPeakTime > 0)
        fitter->SetInitialPeakTime(mira::kInitialPeakTime);
    fitter->SetInitialRiseTime(mira::kInitialRiseTime);
    fitter->SetInitialDecayTime(mira::kInitialDecayTime);
    fitter->SetParametersToFit(mira::kParametersToFit);
    Clear();
}

void mira::FitterClass::Insert()
{
    global_output_->insert(global_output_->end(), output_.begin(), output_.end());
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

void mira::FitterClass::FitSingleBatch(const std::vector<mira::EventData> &input)
{
    if (input.empty())
        return;

    int n_fits = 0;
    for (const auto &evt : input)
    {
        n_fits = n_fits + evt.data_.size();
    }

    const auto n_samples = input.at(0).data_.at(0).size_;

    if (!fitter || n_fits != n_fits_)
    {
        Init(n_fits, n_samples);
    }

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
            if (bg_fitter_)
                bg_fitter_->AddPulse(pulse, ch_data.ch_);
        }
    }

    std::vector<std::vector<float>> pulse_vec;
    if (bg_fitter_)
    {
        bg_fitter_->PoolFit();
        // bg_fitter_->SubtractBGBuffer();
        for (int i = 0; i < pulse_vec.size(); ++i)
        {
            fitter->AddPulse(pulse_vec[i], ch_vec[i]);
        }
    }
    else
    {
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
                fitter->AddPulse(pulse, ch_data.ch_);
            }
        }
    }

    fitter->PoolFit();

    mira::OutputData result;
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
    return;
}