cmake_minimum_required(VERSION 2.8.9)
project(directory_test)

# To automatically run MOC when building(Meta Object Compiler)
set(CMAKE_AUTOMOC ON)

# To automatically run UIC when building(User Interface Compiler)
set(CMAKE_AUTOUIC ON)

# To automatically run RCC when building(Resource Compiler)
set(CMAKE_AUTORCC ON)

include_directories(include)

#Can manually add the sources using the set command as follows:
set(SOURCES src/watch.cpp src/camera_manager.cpp src/objectdetection.cpp 
			src/objecttracker.cpp src/objectdetection_yolo.cpp src/objectdetection_yolotiny.cpp
			src/objectdetection_ssdcaffe.cpp src/objectdetection_gender_caffe.cpp src/objectdetection_age_caffe.cpp
			src/facerecognition.cpp src/qt/watch_qt_main.cpp src/qt/watch_qt.cpp)

find_package(OpenCV)
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Widgets)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "    source path: ${CMAKE_CURRENT_SOURCE_DIR}")

#For the shared library:
set ( PROJECT_LINK_LIBS ${OpenCV_LIBS} )
include_directories(${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/json)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

link_directories( ${OpenCV_LIBS} )

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} -Wall -Wextra -pedantic -std=c++14 -g")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

file(GLOB ${SOURCES} ${SOURCES})

add_executable(watch ${SOURCES})

# Link your application with OpenCV libraries
target_link_libraries(watch ${OpenCV_LIBS} Qt5::Core Qt5::Gui Qt5::Widgets)
