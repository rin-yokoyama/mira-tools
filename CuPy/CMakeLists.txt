find_package(pybind11)
find_package(Python3 COMPONENTS Interpreter Development NumPy)

add_executable(test test.cpp)
target_include_directories(test PRIVATE ${pybind11_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test ${Python3_LIBRARIES} ${pybind11_LIBRARIES})