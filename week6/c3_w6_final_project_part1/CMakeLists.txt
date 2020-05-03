cmake_minimum_required(VERSION 3.16)
project(c3_w6_final_project_part1)

set(CMAKE_CXX_STANDARD 17)

add_executable(c3_w6_final_project_part1
        inverted_index.cpp inverted_index.h
        iterator_range.h
        main.cpp
        parse.cpp parse.h
        search_server.cpp search_server.h
        test_runner.h
        utils.cpp utils.h)
