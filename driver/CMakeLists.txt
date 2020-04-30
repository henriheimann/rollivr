project("ovr-wheelchair-controller-driver")

# Solution
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

# Find precompiled serial library
add_subdirectory(libraries/serial)

# Wheelchair Driver
set(DRIVER_NAME "wheelchair")
set(TARGET_NAME "${DRIVER_NAME}_driver")
add_library("${TARGET_NAME}" SHARED
        "driver_files/src/WheelchairController.cpp"
        "driver_files/src/WheelchairController.h"
        "driver_files/src/WheelchairDriver.cpp"
        "driver_files/src/WheelchairDriver.h"
        "driver_files/src/WheelchairDriverFactory.cpp"
        "driver_files/src/WheelchairDriverFactory.h"
)

target_include_directories("${TARGET_NAME}" PUBLIC "${OPENVR_INCLUDE_DIR}")
target_include_directories("${TARGET_NAME}" PUBLIC "driver_files/src")
target_link_libraries("${TARGET_NAME}" PUBLIC ${OPENVR_LIBRARY} serial)
set_property(TARGET "${TARGET_NAME}" PROPERTY CXX_STANDARD 17)

# Copy driver assets to output folder
add_custom_command(
    TARGET ${TARGET_NAME}
    PRE_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/driver_files/driver/
    $<TARGET_FILE_DIR:${TARGET_NAME}>
)

# Copy dll to output folder
add_custom_command(
    TARGET ${TARGET_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy 
    $<TARGET_FILE:${TARGET_NAME}>
    $<TARGET_FILE_DIR:${TARGET_NAME}>/${DRIVER_NAME}/bin/${PLATFORM_NAME}${PROCESSOR_ARCH}/$<TARGET_FILE_NAME:${TARGET_NAME}>
)