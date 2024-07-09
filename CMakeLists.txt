cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_GLAGS -std=c++17)
PROJECT(GenPointCloud)
#set(Eigen3_DIR D:/ProgramFiles/Eigen3/share/eigen3/cmake)

link_directories("/usr/local/lib/") # 链接库目录
aux_source_directory(./src SRCS) # 添加源代码路径

set(ENV{PKG_CONFIG_PATH} /usr/lib/x86_x64-linux-gnu/pkgconfig)
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENCV4 REQUIRED IMPORTED_TARGET opencv4)
find_package(Eigen3 REQUIRED) # 添加Eigen3依赖

# if(${OPENCV_FOUND})
#     MESSAGE(${PKG_CONFIG_EXECUTABLE})
#     MESSAGE(${OPENCV4_LIBRARIES})
#     MESSAGE(${OPENCV4_LINK_LIBRARIES})
# endif()

include_directories("/usr/include/opencv4/")
include_directories("./include")
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${OpenCV4_INCLUDE_DIRS})


add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} ${EIGEN3_LIBS})
target_link_libraries(${PROJECT_NAME} ${OpenCV4_LIBS})

target_link_libraries(${PROJECT_NAME} stdc++fs) # 链接 stdc++fs