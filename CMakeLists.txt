cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(slurmclient)

include(cmake/CPM.cmake)

include_directories(include)

add_executable(main src/main.cpp)
target_link_libraries(main slurm)
