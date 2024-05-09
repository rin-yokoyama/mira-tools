#include "mira_Parquet.hpp"

mira::RawDataWriter::RawDataWriter()
{
    // Define the memory pool
    pool = arrow::default_memory_pool();

    // Define the structure for the inner list items
    auto efn_field = arrow::field("efn", arrow::int32());
    auto ch_field = arrow::field("ch", arrow::int32());
    auto waveform_field = arrow::field("waveform", arrow::list(arrow::float32())); // 'c' is a list of floats
    struct_type_ = arrow::struct_({efn_field, ch_field, waveform_field});

    // Define schema
    schema_ = arrow::schema({arrow::field("event_id", arrow::int32()),
                             arrow::field("ts", arrow::int64()),
                             arrow::field("data", arrow::list(struct_type_))});
    // Define Arrow builders
    event_id_builder_ = std::make_shared<arrow::Int32Builder>(pool);
    ts_builder_ = std::make_shared<arrow::Int64Builder>(pool);
    efn_builder_ = std::make_shared<arrow::Int32Builder>(pool);
    channel_builder_ = std::make_shared<arrow::Int32Builder>(pool);
    sample_builder_ = std::make_shared<arrow::FloatBuilder>(pool);
    waveform_builder_ = std::make_shared<arrow::ListBuilder>(pool, sample_builder_);
    std::vector<std::shared_ptr<arrow::ArrayBuilder>> field_builders;
    field_builders.emplace_back(efn_builder_);
    field_builders.emplace_back(channel_builder_);
    field_builders.emplace_back(waveform_builder_);
    struct_builder_ = std::make_shared<arrow::StructBuilder>(struct_type_, pool, field_builders);
    list_builder_ = std::make_shared<arrow::ListBuilder>(pool, struct_builder_);
}

void mira::RawDataWriter::Fill(const std::vector<mira::EventData> &data)
{
    for (const auto &event : data)
    {
        //  Append Data
        PARQUET_THROW_NOT_OK(event_id_builder_->Append(event.event_id_));
        PARQUET_THROW_NOT_OK(ts_builder_->Append(event.ts_));
        PARQUET_THROW_NOT_OK(list_builder_->Append());
        for (const auto &ch : event.data_)
        {
            PARQUET_THROW_NOT_OK(struct_builder_->Append());
            PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(struct_builder_->field_builder(0))->Append(ch.efn_));
            PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(struct_builder_->field_builder(1))->Append(ch.ch_));
            PARQUET_THROW_NOT_OK(static_cast<arrow::ListBuilder *>(struct_builder_->field_builder(2))->Append());
            for (int i = 0; i < ch.size_; ++i)
            {
                PARQUET_THROW_NOT_OK(sample_builder_->Append(ch.waveform_[i]));
            }
        }
    }
}

std::shared_ptr<arrow::Table> mira::RawDataWriter::GenerateTable(const std::vector<mira::EventData> &data)
{
    Fill(data);
    // Finalize the arrays
    std::shared_ptr<arrow::Array> event_id_array, ts_array, list_array;
    PARQUET_THROW_NOT_OK(event_id_builder_->Finish(&event_id_array));
    PARQUET_THROW_NOT_OK(ts_builder_->Finish(&ts_array));
    PARQUET_THROW_NOT_OK(list_builder_->Finish(&list_array));
    // Create the table
    auto table = arrow::Table::Make(schema_, {event_id_array, ts_array, list_array});

    return table;
}

std::shared_ptr<arrow::RecordBatch> mira::RawDataWriter::GenerateRecordBatch(const std::vector<EventData> &data)
{
    Fill(data);
    // Finalize the arrays
    std::shared_ptr<arrow::Array> event_id_array, ts_array, list_array;
    PARQUET_THROW_NOT_OK(event_id_builder_->Finish(&event_id_array));
    PARQUET_THROW_NOT_OK(ts_builder_->Finish(&ts_array));
    PARQUET_THROW_NOT_OK(list_builder_->Finish(&list_array));
    // Create the record batch
    return arrow::RecordBatch::Make(schema_, list_builder_->length(), {event_id_array, ts_array, list_array});
}

void mira::RawDataWriter::WriteParquetFile(std::string name, std::shared_ptr<arrow::Table> table)
{
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(
        outfile,
        arrow::io::FileOutputStream::Open(name));

    PARQUET_THROW_NOT_OK(
        parquet::arrow::WriteTable(*table, pool, outfile));
}

std::shared_ptr<arrow::Buffer> mira::RawDataWriter::WriteStream(std::shared_ptr<arrow::Table> table)
{
    // std::shared_ptr<arrow::io::BufferOutputStream> sink;
    auto sink = arrow::io::BufferOutputStream::Create().ValueOrDie();
    arrow::TableBatchReader reader(table);
    std::shared_ptr<arrow::RecordBatch> batch;
    reader.ReadNext(&batch);
    auto writer = arrow::ipc::MakeStreamWriter(sink, schema_).ValueOrDie();
    writer->WriteRecordBatch(*batch.get());
    writer->Close();
    return *sink->Finish();
}
