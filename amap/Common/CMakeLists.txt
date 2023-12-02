cmake_minimum_required(VERSION 3.6)

project(esUtil)

MESSAGE(${CMAKE_CURRENT_SOURCE_DIR})

# Include directory
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/Include)

# Header and source files
file(GLOB ESUTIL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h")

file(GLOB ESUTIL_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c")

file(GLOB WIN32_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32/*.c")

MESSAGE(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/Common/Lib)
# Generate target
add_library(${PROJECT_NAME}  STATIC  ${ESUTIL_HEADERS} ${WIN32_SRC} ${ESUTIL_SRC})
