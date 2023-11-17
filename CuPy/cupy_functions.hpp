#ifndef __CUPY_FUNCTIONS__
#define __CUPY_FUNCTIONS__

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/embed.h>
#include <vector>

namespace py = pybind11;

namespace mira
{
    void convert_to_vector(py::array_t<float> input_array, std::vector<float> &output_array)
    {
        // Check if the input array is a 1D array
        if (input_array.ndim() != 1)
        {
            throw std::runtime_error("Input should be a 1D array");
        }

        auto r = input_array.unchecked<1>(); // Unchecked access to array elements
        output_array.resize(r.size());

        for (ssize_t i = 0; i < r.size(); ++i)
        {
            output_array[i] = r(i); // or result[i] = input_array.at(i) for checked access
        }

        return;
    }

    void call_cupy_fft_filter(std::vector<float> &waveform, const float &freq, const float &width)
    {
        py::array_t<float> py_input;
        py_input.resize(py::array::ShapeContainer({waveform.size(), 1}));
        for (size_t i = 0; i < py_input.shape(0); ++i)
        {
            py_input.mutable_at(i, 0) = waveform[i];
        }
        py::scoped_interpreter guard{}; // start the Python interpreter
        auto my_python_module = py::module_::import("cupy_functions");
        auto cupy_fft_filter = my_python_module.attr("cupy_fft_filter");

        py::array_t<float> result = cupy_fft_filter(py_input, freq, width).cast<py::array_t<float>>();
        mira::convert_to_vector(result, waveform);

        return;
    }

};
#endif