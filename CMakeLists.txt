cmake_minimum_required(VERSION "3.7.1")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Solution
project("OpenVR_Wheelchair_Driver")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(SIZEOF_VOIDP ${CMAKE_SIZEOF_VOID_P})
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PROCESSOR_ARCH "64")
else()
    set(PROCESSOR_ARCH "32")
endif()

if(WIN32)
    set(PLATFORM_NAME "win")
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_NAME "linux")
elseif(APPLE)
    set(PLATFORM_NAME "osx")
endif()

# Find OpenVR
find_package(OpenVR REQUIRED)

# Find Boost
set(BOOST_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system)

# Wheelchair Driver
set(DRIVER_NAME "wheelchair")
set(WHEELCHAIR_PROJECT "driver_${DRIVER_NAME}")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/driver_files/src/*.hpp")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/driver_files/src/*.cpp")
add_library("${WHEELCHAIR_PROJECT}" SHARED "${HEADERS}" "${SOURCES}")

target_include_directories("${WHEELCHAIR_PROJECT}" PUBLIC "${OPENVR_INCLUDE_DIR}")
target_include_directories("${WHEELCHAIR_PROJECT}" PUBLIC "${Boost_INCLUDE_DIRS}")
target_include_directories("${WHEELCHAIR_PROJECT}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/driver_files/src/")
target_link_libraries("${WHEELCHAIR_PROJECT}" PUBLIC ${OPENVR_LIBRARY} ${Boost_LIBRARIES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/driver_files/src" PREFIX "Header Files" FILES ${HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/driver_files/src" PREFIX "Source Files" FILES ${SOURCES})
set_property(TARGET "${WHEELCHAIR_PROJECT}" PROPERTY CXX_STANDARD 17)

# Copy driver assets to output folder
add_custom_command(
    TARGET ${WHEELCHAIR_PROJECT}
    PRE_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_SOURCE_DIR}/driver_files/driver/ 
    $<TARGET_FILE_DIR:${WHEELCHAIR_PROJECT}>
)

# Copy dll to output folder
add_custom_command(
    TARGET ${WHEELCHAIR_PROJECT}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy 
    $<TARGET_FILE:${WHEELCHAIR_PROJECT}>
    $<TARGET_FILE_DIR:${WHEELCHAIR_PROJECT}>/${DRIVER_NAME}/bin/${PLATFORM_NAME}${PROCESSOR_ARCH}/$<TARGET_FILE_NAME:${WHEELCHAIR_PROJECT}>
)