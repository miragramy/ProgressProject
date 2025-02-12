cmake_minimum_required(VERSION 3.14)

project(ProgressProject)
set(CMAKE_CXX_STANDARD 17)

# Find packages
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Include header files
include_directories(headers)

# Collect all source files in the src/ folder
file(GLOB SOURCES "src/*.cpp")

# Create the executable from the source files
add_executable(ProgressProject ${SOURCES})

# Link the executable to the libraries
target_link_libraries(ProgressProject PRIVATE CURL::libcurl)
target_link_libraries(ProgressProject PRIVATE nlohmann_json::nlohmann_json)