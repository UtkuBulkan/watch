cmake_minimum_required(VERSION 2.8.9)
project(directory_test)

include_directories(include)

#Can manually add the sources using the set command as follows:
set(SOURCES src/watch.cpp)

set (OpenCV_DIR /home/utku/opencv4/opencv/build/)

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "    source path: ${CMAKE_CURRENT_SOURCE_DIR}")

#For the shared library:
set ( PROJECT_LINK_LIBS ${OpenCV_LIBS} )
include_directories(/usr/local/include/opencv ${CMAKE_CURRENT_SOURCE_DIR}/src)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

link_directories( /usr/local/lib /usr/lib/x86_64-linux-gnu)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} -Wall -Wextra -pedantic -std=c++14 -g")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

file(GLOB SOURCES "src/watch.cpp")

add_executable(Manager ${SOURCES})

# Link your application with OpenCV libraries
target_link_libraries(watch ${OpenCV_LIBS} tensorflow ssl crypto)
