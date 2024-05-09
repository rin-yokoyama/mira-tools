add_executable(testConsumer testConsumer.cpp)
target_link_libraries(testConsumer rdkafka)

add_executable(stream_decoder decode_stream_data_main.cpp)
target_include_directories(stream_decoder PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${CMAKE_SOURCE_DIR}/parquet)
target_link_libraries(stream_decoder rdkafka MiraDecoder MiraParquet arrow parquet)

add_executable(file2stream stream_from_rawdata_file.cpp)
target_include_directories(file2stream PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${CMAKE_SOURCE_DIR}/parquet)
target_link_libraries(file2stream rdkafka MiraDecoder MiraParquet arrow parquet)

install(TARGETS stream_decoder file2stream
    DESTINATION bin
)

#if(BUILD_GPUFIT)
    add_executable(file2streamgpu stream_from_rawdata_file_gpufit.cpp mira_ParquetGPU.cpp)
    target_include_directories(file2streamgpu PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${CMAKE_SOURCE_DIR}/parquet ${CMAKE_SOURCE_DIR}/gpufit ${CMAKE_SOURCE_DIR}/streaming ${GPUFIT_INSTALL_PATH})
    target_link_libraries(file2streamgpu rdkafka MiraDecoder MiraParquet arrow parquet Gpufit Cpufit GpufitInterface MiraGpufit)

    add_executable(stream_fitter decode_stream_data_gpu_main.cpp mira_ParquetGPU.cpp)
    target_include_directories(stream_fitter PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${CMAKE_SOURCE_DIR}/parquet ${CMAKE_SOURCE_DIR}/gpufit ${CMAKE_SOURCE_DIR}/streaming ${GPUFIT_INSTALL_PATH})
    target_link_libraries(stream_fitter rdkafka MiraDecoder MiraParquet arrow parquet Gpufit Cpufit GpufitInterface MiraGpufit)
#endif()