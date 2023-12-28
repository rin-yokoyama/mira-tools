#include "mira_Parquet.hpp"

std::shared_ptr<arrow::Table> mira::generate_table(const std::vector<mira::EventData> &data)
{
    // Define the memory pool
    pool = arrow::default_memory_pool();

    // Define data builders
    arrow::Int32Builder event_id_builder(pool);
    arrow::Int64Builder ts_builder(pool);
    arrow::Int32Builder ch_id_builder(pool);
    arrow::Int32Builder efn_builder(pool);
    auto waveform_sample_builder = std::make_shared<arrow::FloatBuilder>(pool);
    arrow::ListBuilder waveform_builder(pool, waveform_sample_builder);

    for (const auto &event : data)
    {
        for (const auto &ch : event.data_)
        {
            //  Append Data
            PARQUET_THROW_NOT_OK(event_id_builder.Append(event.event_id_));
            PARQUET_THROW_NOT_OK(ts_builder.Append(event.ts_));
            PARQUET_THROW_NOT_OK(ch_id_builder.Append(ch.ch_));
            PARQUET_THROW_NOT_OK(efn_builder.Append(ch.efn_));
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
    std::shared_ptr<arrow::Array> ch_id_array;
    PARQUET_THROW_NOT_OK(ch_id_builder.Finish(&ch_id_array));
    std::shared_ptr<arrow::Array> efn_array;
    PARQUET_THROW_NOT_OK(efn_builder.Finish(&efn_array));
    std::shared_ptr<arrow::Array> waveform_array;
    PARQUET_THROW_NOT_OK(waveform_builder.Finish(&waveform_array));

    // Define schema
    std::shared_ptr<arrow::Schema> schema = arrow::schema({arrow::field("event_id", arrow::int32()),
                                                           arrow::field("ts", arrow::int64()),
                                                           arrow::field("ch", arrow::int32()),
                                                           arrow::field("efn", arrow::int32()),
                                                           arrow::field("waveform", arrow::list(arrow::float32()))});

    // Create a table
    std::shared_ptr<arrow::Table> table = arrow::Table::Make(schema, {event_id_array, ts_array, ch_id_array, efn_array, waveform_array});

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
