cmake_minimum_required(VERSION 3.16)

project(MutuallyExclusiveFunction VERSION 0.1 LANGUAGES CXX)

# 启用Qt的自动编译工具（MOC, UIC, RCC），简化构建流程[6](@ref)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt库
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core)  # 增加了Core模块
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core)

# 集中管理源文件
set(PROJECT_SOURCES
    main.cpp
    FunctionalMutualExclusionHandle.cpp
)

# 集中管理头文件（用于MOC处理）
set(PROJECT_HEADERS
    Singleton.h
    FunctionalMutualExclusionHandle.h
)

# 创建应用程序目标
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MutuallyExclusiveFunction
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )
else()
    # 统一使用add_executable，移除Android特殊处理（如非必要）
    add_executable(MutuallyExclusiveFunction
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        TestFunction.h
        TestFunction.cpp
    )
endif()

set_source_files_properties(main.cpp PROPERTIES SKIP_AUTOMOC ON)

# 链接Qt库
target_link_libraries(MutuallyExclusiveFunction PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
)

# 设置目标属性
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MutuallyExclusiveFunction)
endif()

set_target_properties(MutuallyExclusiveFunction PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 安装规则
include(GNUInstallDirs)
install(TARGETS MutuallyExclusiveFunction
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Qt6的最终化处理
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MutuallyExclusiveFunction)
endif()
