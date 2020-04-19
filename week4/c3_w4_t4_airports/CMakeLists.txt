# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#
cmake_minimum_required (VERSION 3.8)

project ("c3_w4_t4_airports")

#set(CMAKE_C_COMPILER gcc)
#set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 17)

# Включите подпроекты.
add_executable (c3_w4_t4_airports src/c3_w4_t4_airports.cpp)
