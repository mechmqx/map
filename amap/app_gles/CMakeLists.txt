cmake_minimum_required(VERSION 3.6)

# Include directory
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/amap
    ${CMAKE_SOURCE_DIR}/Common/Include
    ${CMAKE_SOURCE_DIR}/3rdParty/stb)

# Header and source files
file(GLOB MAP_HEADERS
    "${CMAKE_SOURCE_DIR}/amap/*.h"
    "${CMAKE_SOURCE_DIR}/3rdParty/stb/*.h")

file(GLOB MAP_SRC
    "${CMAKE_SOURCE_DIR}/amap/*.c*"
    "${CMAKE_SOURCE_DIR}/app_gles/*.c*")

# Generate target
add_executable(${PROJECT_NAME} WIN32
    ${MAP_HEADERS}
    ${MAP_SRC})

MESSAGE(${CMAKE_SOURCE_DIR})

# Link
target_link_libraries(${PROJECT_NAME} debug     ${CMAKE_SOURCE_DIR}/Common/Lib/Debug/esUtil.lib)
target_link_libraries(${PROJECT_NAME} optimized ${CMAKE_SOURCE_DIR}/Common/Lib/Release/esUtil.lib)
target_link_libraries(${PROJECT_NAME} debug     ${CMAKE_SOURCE_DIR}/3rdParty/GLESv2/lib/libEGL.lib)
target_link_libraries(${PROJECT_NAME} optimized ${CMAKE_SOURCE_DIR}/3rdParty/GLESv2/lib/libEGL.lib)
target_link_libraries(${PROJECT_NAME} debug     ${CMAKE_SOURCE_DIR}/3rdParty/GLESv2/lib/libGLESv2.lib)
target_link_libraries(${PROJECT_NAME} optimized ${CMAKE_SOURCE_DIR}/3rdParty/GLESv2/lib/libGLESv2.lib)


# Others
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

target_compile_options(${PROJECT_NAME} PRIVATE /Wall)

if (MSVC)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${PROJECT_NAME} PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()
