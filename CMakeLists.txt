cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(slurmclient)

add_executable(main src/main.cpp)
target_link_libraries(main slurm)
