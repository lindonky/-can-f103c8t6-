
cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME F103_yuntai)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        Core/BSP/PID/pid.c
        Core/BSP/PID/pid.h
        Core/BSP/GIMBAL/gimbal.c
        Core/BSP/GIMBAL/gimbal.h
        Core/BSP/K230/k230_parser.c
        Core/BSP/K230/k230_parser.h)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined library search paths
)

# ==========================================================
# 自动收集并包含 Core/BSP 目录下的所有源文件和头文件
# ==========================================================
# 1. 定义你的 BSP 目录路径
set(BSP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Core/BSP)

# 2. 自动递归收集 BSP 目录下所有的 .c 文件
file(GLOB_RECURSE BSP_SOURCES CONFIGURE_DEPENDS
        "${BSP_DIR}/*.c"
)

# 3. 将自动收集到的 .c 文件添加到编译目标中
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        ${BSP_SOURCES}
        # 注意：这行下面不需要再手动写 Core/BSP/OLED/OLED.c 了，它会被自动抓取！
)

# 4. 自动递归收集 BSP 目录下所有的 .h 头文件
file(GLOB_RECURSE BSP_HEADERS CONFIGURE_DEPENDS
        "${BSP_DIR}/*.h"
)

# 5. 提取这些头文件所在的文件夹路径，并去重
set(BSP_INCLUDE_DIRS)
foreach(_header IN LISTS BSP_HEADERS)
    get_filename_component(_dir "${_header}" DIRECTORY)
    list(APPEND BSP_INCLUDE_DIRS "${_dir}")
endforeach()
list(REMOVE_DUPLICATES BSP_INCLUDE_DIRS)

# 6. 将提取出的文件夹路径加入 Include 搜索路径
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${BSP_INCLUDE_DIRS}
)
# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
        stm32cubemx

        # Add user defined libraries
)
