cmake_minimum_required(VERSION "3.7.1")

project("ovr-wheelchair-controller")

# Add custom CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# ----------------------------------- Find OpenVR -----------------------------------
find_package(OpenVR REQUIRED)

# ----------------------------------- Subdirectories --------------------------------
add_subdirectory("driver")
add_subdirectory("overlay")