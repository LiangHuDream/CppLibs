cmake_minimum_required(VERSION 3.28)

project(stl_test)


set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)
set(CMAKE_CXX_STANDARD 20)

link_directories(${PROJECT_ROOT_DIR}/lib)
include_directories(${PROJECT_ROOT_DIR}/lib)
include_directories(${PROJECT_ROOT_DIR}/STL)

add_executable(stl_test 
    ./it/test_simple_allocator_it.cpp
    ./per/test_simple_allocator_perf.cpp
    # ./st/test_simple_allocator_st.cpp
    ./ut/simple_allocator_ut.cpp
)

target_link_libraries(stl_test gtest gtest_main gmock pthread)
