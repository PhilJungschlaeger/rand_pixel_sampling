cmake_minimum_required(VERSION 2.8.9)
project (rand_sample_store)
find_package(OpenCV)
#Bring the headers, such as Student.h into the project
include_directories(include)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "source/*.cpp")

add_executable(rand_sample_store ${SOURCES})
target_link_libraries( rand_sample_store ${OpenCV_LIBS} )

