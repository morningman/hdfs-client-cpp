cmake_minimum_required(VERSION 3.10)
project(hdfs-client VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(JNI REQUIRED)
find_package(Threads REQUIRED)

# Set include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${JNI_INCLUDE_DIRS}
    # Custom path to hdfs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/installed/include/hadoop_hdfs
)

# Set static library path
set(HDFS_LIB ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/installed/lib/hadoop_hdfs/native/libhdfs.a)

# Add source files
set(SOURCES 
    src/main.cpp
    src/hdfs_client.cpp
    src/hdfs_builder.cpp
    src/config_loader.cpp
)

# Create executable
add_executable(hdfs_client ${SOURCES})

# Link libraries statically
target_link_libraries(hdfs_client
    ${HDFS_LIB}
    ${JNI_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Installation
install(TARGETS hdfs_client DESTINATION bin) 