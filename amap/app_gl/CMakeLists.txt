cmake_minimum_required(VERSION 3.6)

set(prj_name amap_gl)

add_definitions(-DUSE_OPENGL_MODEN=1)

# Include directory
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/amap
    ${CMAKE_SOURCE_DIR}/app_gl
    ${CMAKE_SOURCE_DIR}/3rdParty/includes
    ${CMAKE_SOURCE_DIR}/3rdParty/stb)

# Header and source files
file(GLOB MAP_GL_HEADERS
    "${CMAKE_SOURCE_DIR}/amap/*.h"
    "${CMAKE_SOURCE_DIR}/app_gl/*.h"
    "${CMAKE_SOURCE_DIR}/3rdParty/stb/*.h")

file(GLOB MAP_GL_SRC
    "${CMAKE_SOURCE_DIR}/amap/*.c*"
    "${CMAKE_SOURCE_DIR}/app_gl/*.c*")

    
# Generate target
add_executable(${prj_name}
    ${MAP_GL_HEADERS}
    ${MAP_GL_SRC})

MESSAGE(${CMAKE_SOURCE_DIR})

# Link
target_link_libraries(${prj_name} debug     ${CMAKE_SOURCE_DIR}/3rdParty/glfw/lib/glfw3.lib)
target_link_libraries(${prj_name} optimized ${CMAKE_SOURCE_DIR}/3rdParty/glfw/lib/glfw3.lib)
target_link_libraries(${prj_name} debug     ${CMAKE_SOURCE_DIR}/3rdParty/glfw/lib/glew32s.lib)
target_link_libraries(${prj_name} optimized ${CMAKE_SOURCE_DIR}/3rdParty/glfw/lib/glew32s.lib)
target_link_libraries(${prj_name} debug     ${CMAKE_SOURCE_DIR}/3rdParty/glfw/lib/SOIL.lib)
target_link_libraries(${prj_name} optimized ${CMAKE_SOURCE_DIR}/3rdParty/glfw/lib/SOIL.lib)
target_link_libraries(${prj_name} debug     ${CMAKE_SOURCE_DIR}/3rdParty/glfw/lib/irrKlang.lib)
target_link_libraries(${prj_name} optimized ${CMAKE_SOURCE_DIR}/3rdParty/glfw/lib/irrKlang.lib)


# Others
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${prj_name})

set_target_properties(${prj_name} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set_target_properties(${prj_name} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

target_compile_options(${prj_name} PRIVATE /Wall)

if (MSVC)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${prj_name} PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()
