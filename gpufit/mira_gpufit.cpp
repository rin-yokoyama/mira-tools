#include "mira_gpufit.hpp"

void mira::gpufit_multithread(
    const std::vector<mira::EventData> &input,
    std::vector<mira::OutputData> &output,
    int n_threads,
    int batch_size,
    int pool_size,
    int n_gpu)
{
    if (n_threads < 1)
    {
        std::cout << "[gpufit_multithread()]: Returning because n_threads < 1" << std::endl;
        return;
    }

    std::cout << "[gpufit_multithread()]: n_threads=" << n_threads
              << ", batch_size=" << batch_size << ", n_pool_workers=(" << n_gpu
              << " gpu, " << pool_size - n_gpu << " cpu)" << std::endl;

    FitThreadPool::Create(pool_size, n_gpu);
    std::vector<mira::FitterClass *> fitter_vec;
    for (int i = 0; i < n_threads; ++i)
    {
        fitter_vec.emplace_back(new mira::FitterClass(&output));
    }
    std::vector<std::future<void>> future_vec(n_threads);
    std::vector<bool> thread_avail(n_threads, true);

    auto nextFitter = [&fitter_vec, &future_vec, &thread_avail](const std::vector<mira::EventData> &batch_input)
    {
        std::rotate(fitter_vec.begin(), fitter_vec.begin() + 1, fitter_vec.end());
        std::rotate(future_vec.begin(), future_vec.begin() + 1, future_vec.end());
        std::rotate(thread_avail.begin(), thread_avail.begin() + 1, thread_avail.end());
        future_vec.back() = std::async(&mira::FitterClass::FitSingleBatch, fitter_vec.back(), batch_input);
        std::cout << "fitter[" << fitter_vec.back()->GetId() << "] is started. Size = " << batch_input.size() << std::endl;
        thread_avail.back() = false;
        if (!thread_avail[0])
        {
            std::cout << "Waiting for fitter[" << fitter_vec[0]->GetId() << "]" << std::endl;
            future_vec[0].wait();
            std::cout << "fitter[" << fitter_vec[0]->GetId() << "] returned" << std::endl;
            fitter_vec[0]->Insert();
            thread_avail[0] = true;
        }
    };

    std::vector<mira::EventData>::const_iterator itr_first = input.begin();
    auto itr_last = [&batch_size](std::vector<mira::EventData>::const_iterator itr)
    { return itr + batch_size; };
    while (itr_last(itr_first) < input.end())
    {
        nextFitter(std::vector<mira::EventData>(itr_first, itr_last(itr_first)));
        itr_first = itr_last(itr_first);
    }
    nextFitter(std::vector<mira::EventData>(itr_first, input.end()));
    for (int i = 0; i < future_vec.size(); ++i)
    {
        if (!thread_avail[i])
        {
            std::cout << "Waiting for fitter[" << fitter_vec[i]->GetId() << "]" << std::endl;
            future_vec[i].wait();
            fitter_vec[i]->Insert();
        }
    }
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
