cmake_minimum_required(VERSION 3.10)
project(DistributedTaskQueue)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-D_XOPEN_SOURCE)

# Find Poco packages
find_package(Poco REQUIRED Foundation Net Data DataPostgreSQL JSON)
# # Find PostgreSQL package for DataPostgreSQL dependency
find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})

# Create library
add_library(taskqueue_lib
    src/Task.cpp
    src/TaskQueue.cpp
    src/Worker.cpp
    src/DatabaseManager.cpp
    src/LoadBalancer.cpp
    src/TaskDistributor.cpp
)

# Include directories
target_include_directories(taskqueue_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${Poco_INCLUDE_DIRS}
)

# Link Poco libraries
target_link_libraries(taskqueue_lib
    PRIVATE
    Poco::Foundation
    Poco::Net
    Poco::Data
    Poco::DataPostgreSQL
    ${PostgreSQL_LIBRARIES}
    Poco::JSON
)
# Add executables
add_executable(TaskQueueServer src/main.cpp)
add_executable(WorkerNode src/WorkerNode.cpp)

# Link executables with the library
target_link_libraries(TaskQueueServer PRIVATE taskqueue_lib)
target_link_libraries(WorkerNode PRIVATE taskqueue_lib)


# find . -type f -exec echo "===== {} =====" \; -exec cat {} \;