#ifndef __MIRA_FITTER_CLASS__
#define __MIRA_FITTER_CLASS__

#include "Interfaces/PulseFitInterface.hpp"
#include "Interfaces/BGFitInterface.hpp"
#include "mira_decoder.hpp"
#include "mira_constants.h"
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
        FitterClass(std::vector<mira::OutputData> *output)
        {
            fitter_id_ = static_id_++;
            global_output_ = output;
            bg_range_ = {0, 0};
        }
        virtual ~FitterClass()
        {
            if (fitter)
                delete fitter;
            fitter = nullptr;
            if (bg_fitter_)
                delete bg_fitter_;
            bg_fitter_ = nullptr;
        }

        void Clear();
        void Init(int n_fits, int n_samples);
        void Insert();
        int GetId() const { return fitter_id_; }
        void FitSingleBatch(const std::vector<mira::EventData> &input);
        void SetBGRange(const std::pair<int, int> &range)
        {
            bg_range_ = range;
        }

        std::vector<mira::OutputData> *global_output_;
        std::vector<mira::OutputData> output_;
        PulseFitInterface *fitter = nullptr;
        std::vector<int> ch_vec;
        std::vector<int> efn_vec;
        std::vector<int> event_id_vec;
        std::vector<u_int64_t> ts_vec;

    protected:
        static int static_id_;
        int fitter_id_;
        int n_fits_ = 0;
        BGFitInterface *bg_fitter_ = nullptr;
        std::pair<int, int> bg_range_;
    };
};

#endif /**__MIRA_FITTER_CLASS__*/