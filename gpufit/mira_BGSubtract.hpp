#ifndef __MIRA_BG_SUBTRACT__
#define __MIRA_BG_SUBTRACT__
#include "Interfaces/BGFitInterface.hpp"
#include "mira_decoder.hpp"
#include "mira_FitterClass.hpp"

namespace mira
{
    struct BGSubtractedData
    {
        std::vector<float> pulse;
        mira::OutputData fit;
    };
    std::vector<mira::BGSubtractedData> subtract_bg(std::vector<mira::EventData> &input, const std::pair<int, int> &range);

    void write_bg_subtracted_data_to_json(std::ofstream &ofs, const std::vector<mira::BGSubtractedData> &data);
};

#endif //__MIRA_BG_SUBTRACT__