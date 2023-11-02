cmake_minimum_required(VERSION 3.11)
project(mira-tools)
add_subdirectory(decoder)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/thismira-tools.sh.in"
    "${CMAKE_SOURCE_DIR}/thismira-tools.sh"
    @ONLY
)

install(FILES ${CMAKE_SOURCE_DIR}/thismira-tools.sh DESTINATION bin)