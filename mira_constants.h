#ifndef __MIRA_CONSTANTS__
#define __MIRA_CONSTANTS__

#include <cmath>
#include <vector>
#include <map>

namespace mira
{
    // Constants for background subtraction
    const std::pair<int, int> kBGFitRange = {0, 400};
    const float kInitialBGCycle = 168. / (2. * M_PI);
    const std::vector<int> kBGParametersToFit = {1, 1, 0, 0, 1};

    // Constants for fitting
    const int kNFitAtOnce = 1000;
    const int kNThreads = 2;
    const int kNGpu = 1;
    const int kNCpu = 0;
    const std::vector<int> kPulseFitRange = {10, 210};
    const int kPrePulseRange = 70;
    const float kInitialPeakTime = -1; // negative value for auto detect
    const float kInitialRiseTime = 1;
    const float kInitialDecayTime = 10;

    const std::vector<int> kChannelsToProcess = {0, 1, 2, 3, 4};

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

#endif //__MIRA_CONSTANTS__