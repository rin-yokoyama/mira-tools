add_executable(testConsumer testConsumer.cpp)
target_link_libraries(testConsumer rdkafka)

add_executable(babiesDump babiesDump.cpp)
target_link_libraries(babiesDump rdkafka)

add_executable(stream_decoder decode_stream_data_main.cpp)
target_include_directories(stream_decoder PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${CMAKE_SOURCE_DIR}/parquet)
target_link_libraries(stream_decoder rdkafka MiraDecoder MiraParquet arrow parquet)

add_executable(file2stream stream_from_rawdata_file.cpp)
target_include_directories(file2stream PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${CMAKE_SOURCE_DIR}/parquet)
target_link_libraries(file2stream rdkafka MiraDecoder MiraParquet arrow parquet)

install(TARGETS stream_decoder file2stream babiesDump
    DESTINATION bin
)

if(GPUFIT)
    add_library(MiraStreaming SHARED mira_ParquetGPU.cpp)
    target_link_directories(MiraStreaming PRIVATE ${GPUFIT_INSTALL_PATH}/build/Gpufit ${GPUFIT_INSTALL_PATH}/build/Cpufit ${GPUFIT_INSTALL_PATH}/build/Interfaces)
    target_include_directories(MiraStreaming PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${CMAKE_SOURCE_DIR}/parquet ${CMAKE_SOURCE_DIR}/gpufit ${CMAKE_SOURCE_DIR}/streaming ${GPUFIT_INSTALL_PATH})
    target_link_libraries(MiraStreaming rdkafka MiraDecoder MiraParquet arrow parquet Gpufit Cpufit GpufitInterface MiraGpufit)

    add_executable(file2streamgpu stream_from_rawdata_file_gpufit.cpp mira_ParquetGPU.cpp)
    target_link_directories(file2streamgpu PRIVATE ${GPUFIT_INSTALL_PATH}/build/Gpufit ${GPUFIT_INSTALL_PATH}/build/Cpufit ${GPUFIT_INSTALL_PATH}/build/Interfaces)
    target_include_directories(file2streamgpu PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${CMAKE_SOURCE_DIR}/parquet ${CMAKE_SOURCE_DIR}/gpufit ${CMAKE_SOURCE_DIR}/streaming ${GPUFIT_INSTALL_PATH})
    target_link_libraries(file2streamgpu rdkafka MiraDecoder MiraParquet MiraStreaming arrow parquet Gpufit Cpufit GpufitInterface MiraGpufit)

    add_executable(stream_fitter decode_stream_data_gpu_main.cpp mira_ParquetGPU.cpp)
    target_link_directories(stream_fitter PRIVATE ${GPUFIT_INSTALL_PATH}/build/Gpufit ${GPUFIT_INSTALL_PATH}/build/Cpufit ${GPUFIT_INSTALL_PATH}/build/Interfaces)
    target_include_directories(stream_fitter PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${CMAKE_SOURCE_DIR}/parquet ${CMAKE_SOURCE_DIR}/gpufit ${CMAKE_SOURCE_DIR}/streaming ${GPUFIT_INSTALL_PATH})
    target_link_libraries(stream_fitter rdkafka MiraDecoder MiraParquet MiraStreaming arrow parquet Gpufit Cpufit GpufitInterface MiraGpufit)

install(TARGETS MiraStreaming
    DESTINATION lib
)

install(TARGETS file2streamgpu stream_fitter
    DESTINATION bin
)
endif()