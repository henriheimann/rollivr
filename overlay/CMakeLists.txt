project("ovr-wheelchair-controller-overlay")

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

set(TARGET_NAME overlay)

add_executable(${TARGET_NAME}
  ${SHARED_SRC_FILES}
        main.cpp
        openvroverlaycontroller.cpp
        openvroverlaycontroller.h
        overlaywidget.cpp
        overlaywidget.h
)

add_definitions(${QT_DEFINITIONS})
if(NOT "${CMAKE_BUILD_TYPE}" EQUAL "Debug")
    add_definitions(-DQT_NO_DEBUG)
endif()

target_include_directories("${TARGET_NAME}" PUBLIC "${OPENVR_INCLUDE_DIR}")
target_link_libraries(${TARGET_NAME}
  ${QT_LIBRARIES}
  ${OPENVR_LIBRARY}
  ${CMAKE_DL_LIBS}
  ${EXTRA_LIBS}
)

set_target_properties(${TARGET_NAME}
  PROPERTIES
  COMPILE_FLAGS "${QT_FLAGS}"
)

#setTargetOutputDirectory(${TARGET_NAME})