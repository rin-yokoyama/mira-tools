#ifndef __MIRA_PARQUET_GPU_HPP__
#define __MIRA_PARQUET_GPU_HPP__
#include <iostream>
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>
#include <arrow/pretty_print.h>
#include <arrow/record_batch.h>
#include <arrow/type.h>
#include <parquet/arrow/writer.h>
#include <vector>
#include "mira_decoder.hpp"
#include "mira_gpufit.hpp"
#include "mira_Parquet.hpp"

namespace mira
{
    /**
     * @brief Class for writing Apache Arrow data to a file or buffer stream.
     *
     */

    class FitDataWriter
    {
    public:
        FitDataWriter();
        virtual ~FitDataWriter() {}

        std::shared_ptr<arrow::Table> GenerateTable(const std::vector<mira::OutputData> &data);
        void WriteParquetFile(std::string name, std::shared_ptr<arrow::Table> table);
        std::shared_ptr<arrow::Buffer> WriteStream(std::shared_ptr<arrow::Table> table);

    protected:
        arrow::MemoryPool *pool_ = nullptr;
        void Fill(const std::vector<mira::OutputData> &data);
        std::shared_ptr<arrow::DataType> struct_type_;
        std::shared_ptr<arrow::Schema> schema_;
        std::shared_ptr<arrow::Int32Builder> efn_builder_;
        std::shared_ptr<arrow::Int32Builder> channel_builder_;
        std::shared_ptr<arrow::Int64Builder> event_id_builder_;
        std::shared_ptr<arrow::Int64Builder> ts_builder_;
        std::shared_ptr<arrow::Int32Builder> state_builder_;
        std::shared_ptr<arrow::FloatBuilder> chi_squared_builder_;
        std::shared_ptr<arrow::Int32Builder> n_iteration_builder_;
        std::shared_ptr<arrow::FloatBuilder> param_builder_;
        std::shared_ptr<arrow::ListBuilder> fit_params_builder_;
    };
}

#endif /*__MIRA_PARQUET_HPP__*/