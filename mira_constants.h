#include <vector>
#include <map>

namespace mira
{
    const std::vector<int> kPulseFitRange = {400, 600};
    const int kPrePulseRange = 200;
    const float kInitialPeakTime = -1; // negative value for auto detect
    const float kInitialRiseTime = 10;
    const float kInitialDecayTime = 100;

    // 1: free param, 0: fix param {ampl, t0, rise, decay, baseline}
    const std::vector<int> kParametersToFit = {1, 1, 1, 1, 1};

    // porarity 1: positive, 0: negative
    const std::map<int, int> kChPolarityMap =
        {
            {0, 1},
            {1, 1},
            {2, 1},
            {3, 1},
            {4, 1},
            {5, 1},
            {6, 1},
            {7, 1},
            {8, 1},
            {9, 1}};
};