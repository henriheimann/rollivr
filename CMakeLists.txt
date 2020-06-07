cmake_minimum_required(VERSION "3.16.1")

project("rollivr")

# Add custom CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# ----------------------------------- Find OpenVR -----------------------------------
find_package(OpenVR REQUIRED)

# ----------------------------------- Subdirectories --------------------------------
add_subdirectory("driver")
add_subdirectory("overlay")

set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "RolliVR")
set(CPACK_PACKAGE_VENDOR "Henri Heimann")
set(CPACK_PACKAGE_DESCRIPTION "")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "RolliVR")
set(CPACK_PACKAGE_FILE_NAME "RolliVR-Installer")

include(CPack)