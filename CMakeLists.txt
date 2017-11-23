cmake_minimum_required(VERSION 2.8.9)
project (sample_pic)
project (sample_vid)
project (nointerpretation)
project (distanceinterpretation)
find_package(OpenCV)
#Bring the headers, such as Student.h into the project
include_directories(include)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "source/*.cpp")

add_executable(nointerpretation source/pixel.cpp source/rsf_writer.cpp source/txt_to_draw.cpp)
target_link_libraries( nointerpretation ${OpenCV_LIBS} )

add_executable(distanceinterpretation source/pixel.cpp source/rsf_writer.cpp source/txt_to_dist_draw3.cpp)
target_link_libraries( distanceinterpretation ${OpenCV_LIBS} )

add_executable(sample_pic source/pixel.cpp source/rsf_writer.cpp source/sample_random_pic.cpp)
target_link_libraries( sample_pic ${OpenCV_LIBS} )

add_executable(sample_vid source/pixel.cpp source/rsf_writer.cpp source/sample_random_vid.cpp)
target_link_libraries( sample_vid ${OpenCV_LIBS} )
