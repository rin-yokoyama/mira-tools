add_executable(testConsumer testConsumer.cpp)
target_link_libraries(testConsumer rdkafka)

add_executable(stream_decoder decode_stream_data_main.cpp)
target_include_directories(stream_decoder PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${CMAKE_SOURCE_DIR}/parquet)
target_link_libraries(stream_decoder rdkafka MiraDecoder MiraParquet)

add_executable(file2stream stream_from_rawdata_file.cpp)
target_include_directories(file2stream PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${CMAKE_SOURCE_DIR}/parquet)
target_link_libraries(file2stream rdkafka MiraDecoder MiraParquet)

install(TARGETS stream_decoder file2stream
    DESTINATION bin
)