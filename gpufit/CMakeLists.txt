set(lib_name MiraGpufit)
set(exec_name gpufit_mira_dump)

add_library(${lib_name} SHARED
    mira_BGSubtract.cpp
    mira_FitterClass.cpp
    mira_gpufit.cpp
)

target_link_directories(${lib_name} PRIVATE ${GPUFIT_INSTALL_PATH}/build/Gpufit ${GPUFIT_INSTALL_PATH}/build/Cpufit ${GPUFIT_INSTALL_PATH}/build/Interfaces)
target_link_libraries(${lib_name} PRIVATE Gpufit Cpufit GpufitInterface MiraDecoder)
target_include_directories(${lib_name} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${GPUFIT_INSTALL_PATH})

install(TARGETS ${lib_name}
    DESTINATION lib
)

add_executable(${exec_name} gpufit_mira_data_main.cpp)

target_link_directories(${exec_name} PRIVATE ${GPUFIT_INSTALL_PATH}/build/Gpufit ${GPUFIT_INSTALL_PATH}/build/Cpufit ${GPUFIT_INSTALL_PATH}/build/Interfaces)
target_link_libraries(${exec_name} PRIVATE Gpufit Cpufit GpufitInterface MiraDecoder ${lib_name})
target_include_directories(${exec_name} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${GPUFIT_INSTALL_PATH})

install(TARGETS ${exec_name}
    DESTINATION bin
)

set(exec_name bgsubtract_mira_data)
add_executable(${exec_name} bgsubtract_mira_dump_main.cpp)

target_link_directories(${exec_name} PRIVATE ${GPUFIT_INSTALL_PATH}/build/Gpufit ${GPUFIT_INSTALL_PATH}/build/Cpufit ${GPUFIT_INSTALL_PATH}/build/Interfaces)
target_link_libraries(${exec_name} PRIVATE Gpufit Cpufit GpufitInterface MiraDecoder ${lib_name})
target_include_directories(${exec_name} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder ${GPUFIT_INSTALL_PATH})

install(TARGETS ${exec_name}
    DESTINATION bin
)