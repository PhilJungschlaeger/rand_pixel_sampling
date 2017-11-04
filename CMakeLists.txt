cmake_minimum_required(VERSION 2.8.9)
project (sample)
project (nointerpretation)
find_package(OpenCV)
#Bring the headers, such as Student.h into the project
include_directories(include)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "source/*.cpp")

add_executable(nointerpretation source/pixel.cpp source/rsf_writer.cpp source/txt_to_draw.cpp)
target_link_libraries( nointerpretation ${OpenCV_LIBS} )

add_executable(sample source/pixel.cpp source/rsf_writer.cpp source/sample_random.cpp)
target_link_libraries( sample ${OpenCV_LIBS} )
