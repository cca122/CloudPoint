cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_COMPILER:FILEPATH "/usr/bin/g++")
set(CMAKE_C_COMPILER:FILEPATH "/usr/bin/gcc")
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_STANDARD 17)

PROJECT(GenPointCloud)
aux_source_directory(./src SRCS) # 添加源代码路径

# set(ENV{PKG_CONFIG_PATH} /usr/lib/x86_x64-linux-gnu/pkgconfig)

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(OPENCV4 REQUIRED IMPORTED_TARGET opencv4)

find_package(Eigen3 REQUIRED) # 添加Eigen3依赖
find_package(OpenCV REQUIRED)

include_directories("/usr/include/opencv4/")
include_directories("./include")
include_directories(${EIGEN3_INCLUDE_DIR})
# include_directories(${OpenCV4_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME} ${EIGEN3_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})
target_link_libraries(${PROJECT_NAME} stdc++fs)
