#ifndef __MIRA_PARQUET_HPP__
#define __MIRA_PARQUET_HPP__
#include <iostream>
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/pretty_print.h>
#include <arrow/record_batch.h>
#include <arrow/type.h>
#include <parquet/arrow/writer.h>
#include <vector>
#include "mira_decoder.hpp"

namespace mira
{
    arrow::MemoryPool *pool = nullptr;
    std::shared_ptr<arrow::Table> generate_table(const std::vector<mira::EventData> &data);
    void wirte_parquet_file(std::string file_name, std::shared_ptr<arrow::Table> table);

    void test();
}

#endif /*__MIRA_PARQUET_HPP__*/