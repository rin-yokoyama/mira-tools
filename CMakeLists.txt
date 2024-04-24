cmake_minimum_required(VERSION 3.11)
project(mira-tools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install CACHE PATH "..." FORCE)
endif()

add_subdirectory(decoder)

option(BUILD_GPUFIT "Build and install GPUFIT tools" ON)
option(BUILD_PARQUET "Build parquet output" ON)

if(BUILD_GPUFIT)
    set(GPUFIT_INSTALL_PATH $ENV{HOME}/opt/Gpufit CACHE PATH "Gpufit")
    add_subdirectory(gpufit)
endif()

if(BUILD_PARQUET)
    add_subdirectory(parquet)
endif()

#add_subdirectory(CuPy)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/thismira-tools.sh.in"
    "${CMAKE_SOURCE_DIR}/thismira-tools.sh"
    @ONLY
)

add_subdirectory(streaming)

install(FILES ${CMAKE_SOURCE_DIR}/thismira-tools.sh DESTINATION bin)