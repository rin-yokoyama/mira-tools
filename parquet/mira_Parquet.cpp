#include "mira_Parquet.hpp"

std::shared_ptr<arrow::Table> mira::generate_table(const std::vector<mira::EventData> &data)
{
    // Define the memory pool
    pool = arrow::default_memory_pool();

    // Define the structure for the inner list items
    auto efn_field = arrow::field("efn", arrow::int32());
    auto ch_field = arrow::field("ch", arrow::int32());
    auto waveform_field = arrow::field("waveform", arrow::list(arrow::float32())); // 'c' is a list of floats
    auto struct_type = arrow::struct_({efn_field, ch_field, waveform_field});

    // Define Arrow builders
    auto event_id_builder = std::make_shared<arrow::Int32Builder>(pool);
    auto ts_builder = std::make_shared<arrow::Int64Builder>(pool);
    auto efn_builder = std::make_shared<arrow::Int32Builder>(pool);
    auto channel_builder = std::make_shared<arrow::Int32Builder>(pool);
    auto sample_builder = std::make_shared<arrow::FloatBuilder>(pool);
    auto waveform_builder = std::make_shared<arrow::ListBuilder>(pool, sample_builder);
    std::vector<std::shared_ptr<arrow::ArrayBuilder>> field_builders;
    field_builders.emplace_back(efn_builder);
    field_builders.emplace_back(channel_builder);
    field_builders.emplace_back(waveform_builder);
    auto struct_builder = std::make_shared<arrow::StructBuilder>(struct_type, pool, field_builders);
    auto list_builder = std::make_shared<arrow::ListBuilder>(pool, struct_builder);

    for (const auto &event : data)
    {
        //  Append Data
        PARQUET_THROW_NOT_OK(event_id_builder->Append(event.event_id_));
        PARQUET_THROW_NOT_OK(ts_builder->Append(event.ts_));
        PARQUET_THROW_NOT_OK(list_builder->Append());
        for (const auto &ch : event.data_)
        {
            PARQUET_THROW_NOT_OK(struct_builder->Append());
            PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(struct_builder->field_builder(0))->Append(ch.efn_));
            PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(struct_builder->field_builder(1))->Append(ch.ch_));
            PARQUET_THROW_NOT_OK(static_cast<arrow::ListBuilder *>(struct_builder->field_builder(2))->Append());
            for (int i = 0; i < ch.size_; ++i)
            {
                PARQUET_THROW_NOT_OK(sample_builder->Append(ch.waveform_[i]));
            }
        }
    }

    // Finalize the arrays
    std::shared_ptr<arrow::Array> event_id_array, ts_array, list_array;
    PARQUET_THROW_NOT_OK(event_id_builder->Finish(&event_id_array));
    PARQUET_THROW_NOT_OK(ts_builder->Finish(&ts_array));
    PARQUET_THROW_NOT_OK(list_builder->Finish(&list_array));

    // Create the table
    std::shared_ptr<arrow::Schema> schema = arrow::schema({arrow::field("event_id", arrow::int32()),
                                                           arrow::field("ts", arrow::int64()),
                                                           arrow::field("data", arrow::list(struct_type))});
    auto table = arrow::Table::Make(schema, {event_id_array, ts_array, list_array});

    return table;
}

void mira::wirte_parquet_file(std::string name, std::shared_ptr<arrow::Table> table)
{
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(
        outfile,
        arrow::io::FileOutputStream::Open(name));

    PARQUET_THROW_NOT_OK(
        parquet::arrow::WriteTable(*table, pool, outfile));
}
