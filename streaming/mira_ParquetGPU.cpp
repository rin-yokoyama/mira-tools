#include "mira_ParquetGPU.hpp"

mira::FitDataWriter::FitDataWriter()
{
    // Define the memory pool
    pool_ = arrow::default_memory_pool();

    // Define schema
    schema_ = arrow::schema({arrow::field("efn", arrow::int32()),
                             arrow::field("ch", arrow::int32()),
                             arrow::field("event_id", arrow::int64()),
                             arrow::field("ts", arrow::int64()),
                             arrow::field("state", arrow::int32()),
                             arrow::field("chi_squared", arrow::float32()),
                             arrow::field("n_iteration", arrow::int32()),
                             arrow::field("fit_params", arrow::list(arrow::float32()))});

    // Define Arrow builders
    efn_builder_ = std::make_shared<arrow::Int32Builder>(pool_);
    channel_builder_ = std::make_shared<arrow::Int32Builder>(pool_);
    event_id_builder_ = std::make_shared<arrow::Int64Builder>(pool_);
    ts_builder_ = std::make_shared<arrow::Int64Builder>(pool_);
    state_builder_ = std::make_shared<arrow::Int32Builder>(pool_);
    chi_squared_builder_ = std::make_shared<arrow::FloatBuilder>(pool_);
    n_iteration_builder_ = std::make_shared<arrow::Int32Builder>(pool_);
    param_builder_ = std::make_shared<arrow::FloatBuilder>(pool_);
    fit_params_builder_ = std::make_shared<arrow::ListBuilder>(pool_, param_builder_);
}

void mira::FitDataWriter::Fill(const std::vector<mira::OutputData> &data)
{
    for (const auto &event : data)
    {
        //  Append Data
        PARQUET_THROW_NOT_OK(efn_builder_->Append(event.efn_));
        PARQUET_THROW_NOT_OK(channel_builder_->Append(event.ch_));
        PARQUET_THROW_NOT_OK(event_id_builder_->Append(event.event_id_));
        PARQUET_THROW_NOT_OK(ts_builder_->Append(event.ts_));
        PARQUET_THROW_NOT_OK(state_builder_->Append(event.fit_result_.state_));
        PARQUET_THROW_NOT_OK(chi_squared_builder_->Append(event.fit_result_.chi_squared_));
        PARQUET_THROW_NOT_OK(n_iteration_builder_->Append(event.fit_result_.n_iterations_));
        PARQUET_THROW_NOT_OK(fit_params_builder_->Append());
        for (const auto &param : event.fit_result_.params_)
        {
            PARQUET_THROW_NOT_OK(param_builder_->Append(param));
        }
    }
}

std::shared_ptr<arrow::Table> mira::FitDataWriter::GenerateTable(const std::vector<mira::OutputData> &data)
{
    Fill(data);
    // Finalize the arrays
    std::shared_ptr<arrow::Array> efn_array, ch_array, event_id_array, ts_array, state_array, chisq_array, niter_array, params_array;
    PARQUET_THROW_NOT_OK(efn_builder_->Finish(&efn_array));
    PARQUET_THROW_NOT_OK(channel_builder_->Finish(&ch_array));
    PARQUET_THROW_NOT_OK(event_id_builder_->Finish(&event_id_array));
    PARQUET_THROW_NOT_OK(ts_builder_->Finish(&ts_array));
    PARQUET_THROW_NOT_OK(state_builder_->Finish(&state_array));
    PARQUET_THROW_NOT_OK(chi_squared_builder_->Finish(&chisq_array));
    PARQUET_THROW_NOT_OK(n_iteration_builder_->Finish(&niter_array));
    PARQUET_THROW_NOT_OK(fit_params_builder_->Finish(&params_array));
    // Create the table
    auto table = arrow::Table::Make(schema_, {efn_array, ch_array, event_id_array, ts_array, state_array, chisq_array, niter_array, params_array});

    return table;
}

void mira::FitDataWriter::WriteParquetFile(std::string name, std::shared_ptr<arrow::Table> table)
{
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(
        outfile,
        arrow::io::FileOutputStream::Open(name));

    PARQUET_THROW_NOT_OK(
        parquet::arrow::WriteTable(*table, pool_, outfile));
}

std::shared_ptr<arrow::Buffer> mira::FitDataWriter::WriteStream(std::shared_ptr<arrow::Table> table)
{
    // std::shared_ptr<arrow::io::BufferOutputStream> sink;
    auto sink = arrow::io::BufferOutputStream::Create().ValueOrDie();
    arrow::TableBatchReader reader(table);
    std::shared_ptr<arrow::RecordBatch> batch;
    auto readState = reader.ReadNext(&batch);
    if (readState.ok() && batch.get())
    {
        auto writer = arrow::ipc::MakeStreamWriter(sink, schema_).ValueOrDie();
        writer->WriteRecordBatch(*batch.get());
        writer->Close();
    }
    return *sink->Finish();
}
