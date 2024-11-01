cmake_minimum_required(VERSION 3.10)
project(table2image_example)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -w -lrt -O2 --std=c++11")
get_filename_component(PARENT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
include_directories(${PARENT_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include)
link_directories(${PARENT_DIR}/lib)
add_executable(table2image_example example.cpp)
target_link_libraries(table2image_example table2image pthread dl)

add_executable(excel_proccess_example excel_proccess_example.cpp)
target_link_libraries(excel_proccess_example table2image pthread dl)