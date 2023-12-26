#include "mira_Parquet.hpp"

std::shared_ptr<arrow::Table> mira::generate_table(const std::vector<mira::EventData> &data)
{
    // Define the memory pool
    pool = arrow::default_memory_pool();

    // Define data builders
    arrow::Int32Builder event_id_builder(pool);
    arrow::Int64Builder ts_builder(pool);

    // Define the structure fields
    auto ch_field = arrow::field("ch", arrow::int32());
    auto efn_field = arrow::field("efn", arrow::int32());
    auto waveform_field = arrow::field("waveform", arrow::list(arrow::float32()));
    auto struct_type = arrow::struct_({ch_field, efn_field, waveform_field});

    // Create a StructBuilder
    auto waveform_sample_builder = std::make_shared<arrow::FloatBuilder>(pool);
    arrow::ListBuilder waveform_builder(pool, waveform_sample_builder);
    std::vector<std::shared_ptr<arrow::ArrayBuilder>> field_builders = {
        std::make_shared<arrow::Int32Builder>(pool),
        std::make_shared<arrow::Int32Builder>(pool),
        std::make_shared<arrow::ListBuilder>(pool, waveform_sample_builder)};
    auto ch_data_builder = std::make_shared<arrow::StructBuilder>(struct_type, pool, field_builders);
    arrow::ListBuilder list_builder(pool, ch_data_builder);

    for (const auto &event : data)
    {
        // Append EventData
        PARQUET_THROW_NOT_OK(event_id_builder.Append(event.event_id_));
        PARQUET_THROW_NOT_OK(ts_builder.Append(event.ts_));
        PARQUET_THROW_NOT_OK(list_builder.Append());
        for (const auto &ch : event.data_)
        {
            // Append ChData
            PARQUET_THROW_NOT_OK(ch_data_builder->Append());
            PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(ch_data_builder->field_builder(0))->Append(ch.ch_));
            PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(ch_data_builder->field_builder(1))->Append(ch.efn_));
            PARQUET_THROW_NOT_OK(waveform_builder.Append());
            for (int i = 0; i < ch.size_; ++i)
            {
                PARQUET_THROW_NOT_OK(waveform_sample_builder->Append((float)ch.waveform_[i]));
            }
        }
    }

    // Finalize the array
    std::shared_ptr<arrow::Array> event_id_array;
    PARQUET_THROW_NOT_OK(event_id_builder.Finish(&event_id_array));
    std::shared_ptr<arrow::Array> ts_array;
    PARQUET_THROW_NOT_OK(ts_builder.Finish(&ts_array));
    std::shared_ptr<arrow::Array> list_array;
    PARQUET_THROW_NOT_OK(ch_data_builder->Finish(&list_array));
    PARQUET_THROW_NOT_OK(list_builder.Finish(&list_array));

    // Define schema
    std::shared_ptr<arrow::Schema> schema = arrow::schema({arrow::field("event_id", arrow::int32()),
                                                           arrow::field("ts", arrow::int64()),
                                                           arrow::field("list", arrow::list(struct_type))});

    // Create a table
    std::shared_ptr<arrow::Table> table = arrow::Table::Make(schema, {event_id_array, ts_array, list_array});
    return table;
}

void mira::wirte_parquet_file(std::string name, std::shared_ptr<arrow::Table> table)
{
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(
        outfile,
        arrow::io::FileOutputStream::Open(name));

    PARQUET_THROW_NOT_OK(
        parquet::arrow::WriteTable(*table, pool, outfile, 2048));
}