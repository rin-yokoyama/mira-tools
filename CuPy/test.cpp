#include <iostream>
#include <vector>
#include "cupy_functions.hpp"

int main()
{
    std::vector<float> pulse = {8507, 8507, 8501, 8500, 8509, 8497, 8494, 8497, 8499, 8494, 8501, 8498, 8501, 8497, 8496, 8498, 8499, 8500, 8493, 8493, 8501, 8486, 8492, 8491, 8488, 8490, 8492, 8497, 8487, 8493, 8492, 8489, 8491, 8491, 8490, 8488, 8491, 8483, 8481, 8486, 8487, 8482, 8490, 8490, 8492, 8502, 8493, 8482, 8498, 8496, 8491, 8499, 8498, 8494, 8502, 8493, 8496, 8498, 8496, 8489, 8483, 8502, 8492, 8490, 8502, 8497, 8486, 8500, 8514, 8498, 8496, 8502, 8509, 8494, 8511, 8508, 8502, 8517, 8501, 8509, 8508, 8509, 8509, 8504, 8514, 8506, 8506, 8513, 8509, 8515, 8523, 8513, 8519, 8521, 8516, 8525, 8520, 8522, 8517, 8519, 8524, 8519, 8520, 8532, 8525, 8522, 8521, 8522, 8515, 8526, 8527, 8528, 8523, 8523, 8520, 8522, 8523, 8528, 8530, 8527, 8520, 8528, 8525, 8525, 8521, 8522, 8528, 8516, 8525, 8528, 8519, 8530, 8526, 8518, 8522, 8521, 8526, 8516, 8518, 8524, 8513, 8524, 8512, 8513, 8518, 8516, 8516, 8515, 8511, 8513, 8512, 8509, 8512, 8506, 8518, 8508, 8502, 8502, 8505, 8504, 8510, 8505, 8501, 8509, 8505, 8494, 8500, 8498, 8499, 8489, 8498, 8503, 8509, 8500, 8496, 8493, 8485, 8508, 8496, 8491, 8488, 8489, 8493, 8492, 8490, 8499, 8490, 8485, 8483, 8494, 8492, 8489, 8497, 8495, 8484, 8495, 8493, 8495, 8489, 8497, 8486, 8486, 8490, 8479, 8481, 8480, 8490, 8487, 8482, 8495, 8482, 8495, 8494, 8494, 8489, 8491, 8495, 8493, 8490, 8495, 8490, 8500, 8504, 8499, 8494, 8500, 8505, 8499, 8508, 8501, 8497, 8499, 8504, 8507, 8506, 8508, 8504, 8501, 8507, 8502, 8514, 8514, 8506, 8505, 8511, 8513, 8514, 8513, 8518, 8512, 8513, 8519, 8512, 8515, 8527, 8519, 8519, 8524, 8517, 8521, 8545, 8551, 8487, 8491, 8518, 8523, 8521, 8519, 8531, 8539, 8528, 8531, 8541, 8527, 8525, 8529, 8522, 8532, 8520, 8522, 8525, 8526, 8526, 8517, 8524, 8525, 8533, 8530, 8516, 8532, 8526, 8513, 8530, 8515, 8521, 8521, 8520, 8522, 8516, 8510, 8513, 8514, 8511, 8506, 8508, 8514, 8515, 8507, 8512, 8512, 8509, 8501, 8507, 8509, 8503, 8514, 8507, 8495, 8506, 8505, 8499, 8504, 8495, 8498, 8496, 8490, 8499, 8498, 8493, 8493, 8489, 8503, 8493, 8496, 8505, 8485, 8490, 8490, 8490, 8496, 8487, 8491, 8487, 8475, 8483, 8490, 8493, 8490, 8480, 8489, 8489, 8484, 8488, 8485, 8485, 8484, 8490, 8489, 8484, 8486, 8490, 8479, 8484, 8485, 8484, 8495, 8491, 8496, 8480, 8496, 8498, 8485, 8495, 8492, 8491, 8492, 8505, 8496, 8494, 8499, 8499, 8496, 8497, 8502, 8494, 8498, 8507, 8498, 8515, 8495, 8494, 8512, 8511, 8505, 8505, 8519, 8516, 8508, 8518, 8509, 8512, 8520, 8521, 8512, 8516, 8520, 8520, 8520, 8517, 8525, 8509, 8523, 8522, 8512, 8521, 8524, 8516, 8522, 8520, 8517, 8520, 8522, 8520, 8527, 8529, 8521, 8517, 8535, 8528, 8532, 8521, 8525, 8520, 8523, 8530, 8519, 8518, 8527, 8514, 8521, 8516, 8518, 8517, 8525, 8519, 8523, 8528, 8515, 8524, 8516, 8526, 8515, 8510, 8520, 8519, 8517, 8515, 8517, 8510, 8515, 8514, 8505, 8515, 8509, 8504, 8507, 8510, 8505, 8497, 8512, 8503, 8499, 8151, 8469, 7996, 8453, 8552, 8412, 8457, 8517, 8491, 8495, 8493, 8493, 8496, 8492, 8491, 8493, 8489, 8491, 8491, 8485, 8502, 8489, 8487, 8495, 8480, 8493, 8487, 8483, 8486, 8485, 8487, 8479, 8486, 8482, 8483, 8491, 8487, 8479, 8489, 8489, 8481, 8488, 8489, 8489, 8489, 8488, 8491, 8491, 8486, 8494, 8487, 8483, 8495, 8498, 8507, 8493, 8493, 8492, 8495, 8496, 8501, 8499, 8503, 8494, 8501, 8501, 8484, 8503, 8498, 8499, 8508, 8515, 8508, 8500, 8505, 8511, 8506, 8510, 8517, 8511, 8514, 8512, 8520, 8520, 8513, 8510, 8519, 8514, 8516, 8520, 8524, 8523, 8515, 8511, 8520, 8517, 8521, 8522, 8529, 8525, 8522, 8514, 8516, 8528, 8520, 8512, 8524, 8525, 8520, 8527, 8519, 8521, 8527, 8515, 8515, 8520, 8528, 8519, 8513, 8522, 8520, 8517, 8528, 8520, 8524, 8509, 8518, 8519, 8514, 8533, 8517, 8517, 8521, 8512, 8514, 8522, 8510, 8519, 8511, 8514, 8516, 8503, 8507, 8507, 8513, 8514, 8507, 8507, 8507, 8502, 8499, 8500, 8511, 8499, 8495, 8506, 8496, 8507, 8498, 8499, 8502, 8501, 8490, 8495, 8505, 8498, 8496, 8492, 8493, 8493, 8497, 8502, 8485, 8498, 8491, 8490, 8492, 8498, 8488, 8490, 8487, 8497, 8483, 8493, 8490, 8489, 8484, 8492, 8486, 8496, 8495, 8487, 8490, 8493, 8485, 8493, 8489, 8504, 8495, 8494, 8507, 8480, 8502, 8493, 8490, 8496, 8489, 8497, 8498, 8498, 8496, 8491, 8502, 8498, 8504, 8513, 8498, 8508, 8513, 8502, 8505, 8513, 8513, 8512, 8502, 8517, 8512, 8503, 8510, 8511, 8507, 8511, 8513, 8510, 8510, 8514, 8512, 8520, 8519, 8515, 8517, 8528, 8506, 8521, 8529, 8517, 8517, 8519, 8528, 8529, 8513, 8515, 8530, 8527, 8528, 8521, 8522, 8533, 8525, 8527, 8530, 8522, 8527, 8524, 8528, 8529, 8523, 8531, 8518, 8517, 8521, 8523, 8528, 8524, 8519, 8531, 8520, 8524, 8517, 8522, 8529, 8515, 8519, 8520, 8521, 8524, 8520, 8524, 8513, 8516, 8521, 8507, 8515, 8511, 8510, 8511, 8506, 8504, 8511, 8515, 8508, 8511, 8506, 8515, 8511, 8509, 8505, 8503, 8509, 8502, 8494, 8509, 8501, 8502, 8503, 8495, 8503, 8490, 8499, 8496, 8490, 8496, 8499, 8488, 8497, 8491, 8489, 8505, 8490, 8483, 8494, 8490, 8492, 8488, 8504, 8490, 8485, 8491, 8482, 8488, 8491, 8483, 8498, 8484, 8482, 8484, 8491, 8490, 8487, 8494, 8496, 8481, 8490, 8493, 8500, 8498, 8496, 8494, 8496, 8493, 8498, 8497, 8490, 8496, 8503, 8486, 8496, 8504, 8501, 8499, 8504, 8511, 8498, 8504, 8505, 8511, 8507, 8497, 8513, 8521, 8503, 8509, 8512, 8506, 8514, 8514, 8512, 8512, 8509, 8521, 8513, 8520, 8517, 8514, 8527, 8528, 8517, 8514, 8522, 8522, 8521, 8523, 8523, 8520, 8530, 8529, 8524, 8533, 8528, 8525, 8529, 8523, 8531, 8523, 8526, 8537, 8516, 8530, 8528, 8522, 8530, 8528, 8525, 8526, 8530, 8529, 8526, 8526, 8528, 8520, 8529, 8530, 8519, 8524, 8521, 8522, 8528, 8523, 8518, 8521, 8522, 8520, 8512, 8521, 8516, 8506, 8519, 8509, 8509, 8519, 8511, 8503, 8507, 8515, 8513, 8510, 8509, 8509, 8509, 8508, 8501, 8498, 8503, 8501, 8503, 8503, 8496, 8504, 8504, 8498, 8491, 8502, 8500, 8491, 8494, 8499, 8491, 8498, 8491, 8494, 8498, 8493, 8489, 8489, 8488, 8494, 8486, 8488, 8500, 8485, 8491, 8493, 8489, 8482, 8491, 8491, 8481, 8487, 8483, 8495, 8487, 8493, 8486, 8487, 8492, 8490, 8487, 8485, 8490, 8494, 8494, 8492, 8492, 8495, 8506, 8492, 8492, 8498, 8504, 8504, 8495, 8506, 8497, 8505, 8501, 8496, 8509, 8505, 8496, 8517, 8513, 8503, 8511, 8505, 8512, 8517, 8506, 8513, 8503, 8520, 8516, 8515, 8509, 8516, 8514, 8519, 8517, 8521, 8519, 8521, 8525, 8515};
    mira::call_cupy_fft_filter(pulse, 0.006, 0.001);
    return 0;
}