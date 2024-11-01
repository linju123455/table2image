cmake_minimum_required(VERSION 3.10)
project(table2image)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -w -pthread -lrt -O2 --std=c++11")

# 查找 OpenCV 库
# set(OpenCV_DIR "/home/linju/linju_workspace/cv_test/lib/cmake/opencv5")
# find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs highgui freetype)
# include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/opencv5)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/opencv5/3rdparty)


# 链接 OpenCV 库
# target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

add_library(${PROJECT_NAME} SHARED ITC_table2image.cpp)
target_link_libraries(${PROJECT_NAME}
    libopencv_freetype.a 
    libopencv_highgui.a
    libopencv_imgcodecs.a
    libopencv_imgproc.a
    libopencv_core.a
    libharfbuzz.a 
    libgraphite2.a 
    libfreetype.a
    libbz2.a
    libIlmImf.a 
    liblibopenjp2.a 
    liblibpng.a 
    liblibjpeg-turbo.a 
    liblibtiff.a 
    libittnotify.a 
    liblibwebp.a 
    libippiw.a 
    libippicv.a 
    libzlib.a 
    libxlnt.a
    dl
)

# add_executable(main main.cpp)
# target_link_libraries(main ${PROJECT_NAME})

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/ITC_table2image_deploy")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ITC_table2image.h DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libtable2image.so DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/simhei.ttf DESTINATION ./)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md DESTINATION ./)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/example DESTINATION ./) 
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/popular-fonts-master DESTINATION ./) 
