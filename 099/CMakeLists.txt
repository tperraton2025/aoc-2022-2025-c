cmake_minimum_required(VERSION 3.10.0)

get_filename_component(day ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" day ${day})

set(ProjectId ${YEAR}day${day})
project(
  ${ProjectId}
  VERSION 0.1.0
  LANGUAGES C CXX)

add_executable(${ProjectId} main.c part1.c)
target_include_directories(${ProjectId} PRIVATE ${CMAKE_SOURCE_DIR}
                                                ${CMAKE_SOURCE_DIR}/../lib/inc)

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

set_target_properties(
  ${ProjectId}
  PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(Threads)
target_link_libraries(${ProjectId} libaochelpers m ${CMAKE_THREAD_LIBS_INIT})
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_BUILD_TYPE DEBUG)
add_definitions(-DCONFIG_DAY="${day}" -DCONFIG_YEAR="${YEAR}")


# unit tests deactivated for now
# include(CTest)
# add_test(NAME ${ProjectId}test1
#          COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bin/${ProjectId}"
#                  "${CMAKE_CURRENT_SOURCE_DIR}/data/test1.txt")
# add_test_regex(${ProjectId}test1 "inputs_routine exited normally")
# add_test_regex(${ProjectId}test1 "graphics_routine exited normally")
