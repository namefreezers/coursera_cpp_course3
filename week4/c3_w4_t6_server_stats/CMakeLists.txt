# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#
cmake_minimum_required (VERSION 3.8)

project ("c3_w4_t6_server_stats")

set(CMAKE_CXX_STANDARD 17)

file(GLOB ALL_SRC
    "src/*.h"
    "src/*.cpp"
)

add_executable("c3_w4_t6_server_stats" ${ALL_SRC})
