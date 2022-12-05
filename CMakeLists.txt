cmake_minimum_required(VERSION 3.16)
project(AresCV)

set(CMAKE_CXX_STANDARD 20)

find_package(Python REQUIRED COMPONENTS Development Interpreter)
find_package(OpenCV REQUIRED)
find_package(pybind11 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Python_INCLUDE_DIRS})
include_directories(${pybind11_INCLUDE_DIRS})
include_directories(3rdparty/UltraMultiThread/include)
include_directories(3rdparty)
include_directories(devices)


SET (ALL_SRCS main.cpp devices/serial/serial.cpp devices/serial/serial.h
        autoaim/autoaim.cpp autoaim/autoaim.h
        devices/camera/camera_api.cpp devices/camera/MER131.cpp devices/camera/MER131.h
        autoaim/armor/armor.cpp autoaim/armor/armor.h)

add_executable(AresCV ${ALL_SRCS})
target_link_libraries(AresCV pthread)
target_link_libraries(AresCV Python::Python pybind11::pybind11 ${OpenCV_LIBS} /usr/lib/libgxiapi.so)