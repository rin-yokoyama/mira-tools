set(lib_name MiraDecoder)

add_library(${lib_name} SHARED
    mira_decoder.cpp
)

install(TARGETS ${lib_name}
    DESTINATION lib
)

add_executable(decode_mira_dump decode_mira_data_main.cpp)

target_include_directories(decode_mira_dump PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(decode_mira_dump ${lib_name})

install(TARGETS decode_mira_dump
    DESTINATION bin
)