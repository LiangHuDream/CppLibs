cmake_minimum_required(VERSION 3.10)
project(externDemo VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(${PROJECT_NAME}.out 
    main.cpp
    my_template.cpp
    my_template.h
)

# 如果使用C++20 modules需要添加下面这句（根据实际情况）
# set_target_properties(${PROJECT_NAME} PROPERTIES CXX_SCAN_FOR_MODULES ON)