cmake_minimum_required(VERSION 3.10)
project(externDemo VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}.out 
    main.cpp
    file1.cpp
    file2.cpp
    my_template.cpp
    my_template.hpp
)
