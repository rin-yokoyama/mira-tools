set(lib_name MiraParquet)
set(exec_name mira_dump_parquet)

find_package(Arrow REQUIRED)
find_package(Parquet REQUIRED)

add_library(${lib_name} SHARED
    mira_Parquet.cpp
)

target_link_libraries(${lib_name} PRIVATE MiraDecoder arrow parquet)
target_include_directories(${lib_name} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder)

install(TARGETS ${lib_name}
    DESTINATION lib
)

add_executable(${exec_name} mira_dump_parquet_main.cpp)

target_link_libraries(${exec_name} PRIVATE MiraDecoder ${lib_name} arrow parquet)
target_include_directories(${exec_name} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/decoder)

install(TARGETS ${exec_name}
    DESTINATION bin
)