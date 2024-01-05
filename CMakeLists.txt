cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(slurmclient)

include(cmake/CPM.cmake)

cpmaddpackage("gh:nlohmann/json@3.11.3")

add_executable(main src/main.cpp)
target_link_libraries(main slurm nlohmann_json::nlohmann_json)
