cmake_minimum_required(VERSION 3.6)

project(amap)

# Add preprocess macros
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_subdirectory(Common)
add_subdirectory(app_gles)
#add_subdirectory(app_gl)