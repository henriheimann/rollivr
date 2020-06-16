project("overlay_rollivr")

# ----------------------------------- Find Qt ---------------------------------------
# Set CMake to run moc automatically has needed.
set(CMAKE_AUTOMOC ON)
# Set CMake to handle *.ui generation to avoid using QT5_WRAP_UI() macro.
set(CMAKE_AUTOUIC ON)
# Add the build dir to include paths to find UI's headers.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find Qt5 libraries.
foreach(module Core Gui Widgets)
    find_package(Qt5 REQUIRED COMPONENTS ${module})
    list(APPEND QT_INCLUDE_DIRS "${Qt5${module}_INCLUDE_DIRS}")
    list(APPEND QT_LIBRARIES    "${Qt5${module}_LIBRARIES}")
    list(APPEND QT_FLAGS        "${Qt5${module}_EXECUTABLE_COMPILE_FLAGS}")
    list(APPEND QT_DEFINITIONS  "${Qt5${module}_DEFINITIONS}")
endforeach()

# Correct QT_FLAGS having semicolons, which break the compile step on POSIX
list(REMOVE_DUPLICATES QT_FLAGS)

# Find the windeployqt executable
get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
get_filename_component(QT5_WINDEPLOYQT_EXECUTABLE ${QT5_QMAKE_EXECUTABLE} PATH)
set(QT5_WINDEPLOYQT_EXECUTABLE "${QT5_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")

# ----------------------------------- Find GLM --------------------------------------
find_package(GLM REQUIRED)

# ----------------------------------- Build Process ---------------------------------
set(TARGET_NAME overlay_rollivr)

add_executable(${TARGET_NAME} WIN32
  ${SHARED_SRC_FILES}
        src/main.cpp
        src/RolliVROverlayController.cpp
        src/RolliVROverlayController.h
        src/RolliVROverlayWidget.cpp
        src/RolliVROverlayWidget.h
        src/main.h)

add_definitions(${QT_DEFINITIONS})
if(NOT "${CMAKE_BUILD_TYPE}" EQUAL "Debug")
    add_definitions(-DQT_NO_DEBUG)
endif()

target_include_directories("${TARGET_NAME}" PUBLIC "${OPENVR_INCLUDE_DIR}" "${GLM_INCLUDE_DIR}")
target_link_libraries(${TARGET_NAME}
  ${QT_LIBRARIES}
  ${OPENVR_LIBRARY}
  ${CMAKE_DL_LIBS}
  ${EXTRA_LIBS}
)

set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "${QT_FLAGS}")

# Prevent Debug and Release subdirectories when building with Visual Studio
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}>/rollivr)
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}>/rollivr)
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}>/rollivr)

# ----------------------------------- Installation ----------------------------------

# Include runtime
set(CMAKE_INSTALL_URCT_LIBRARIES TRUE)
include(InstallRequiredSystemLibraries)

# Add binary
install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION . LIBRARY DESTINATION .)

# Add application files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/application_files/ DESTINATION .)

# Include Qt runtime via windeployqt
include(Windeployqt)
windeployqt(${TARGET_NAME} .)

# Manually copy OpenVR dll, depending on build it may reside in ${CMAKE_BUILD_TYPE} subdirectory or not
# TODO: Find a way to automate this
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rollivr/openvr_api.dll DESTINATION .)