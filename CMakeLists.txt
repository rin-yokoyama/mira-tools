cmake_minimum_required(VERSION 3.11)
project(mira-tools)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install CACHE PATH "..." FORCE)
endif()

add_subdirectory(decoder)

option(BUILD_GPUFIT "Build and install GPUFIT tools" ON)

if(BUILD_GPUFIT)
    set(GPUFIT_INSTALL_PATH $ENV{HOME}/opt/Gpufit CACHE PATH "Gpufit")
    add_subdirectory(gpufit)
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/thismira-tools.sh.in"
    "${CMAKE_SOURCE_DIR}/thismira-tools.sh"
    @ONLY
)

install(FILES ${CMAKE_SOURCE_DIR}/thismira-tools.sh DESTINATION bin)