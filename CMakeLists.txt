cmake_minimum_required(VERSION 3.15)
project(Source/Async)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(SOURCE_FILE_FOLDER "Source")

# Librar
add_library(Async
  ${SOURCE_FILE_FOLDER}/Async/Async.hpp
  ${SOURCE_FILE_FOLDER}/Async/TaskLoop.cpp
  ${SOURCE_FILE_FOLDER}/Async/TaskLoop.hpp
  ${SOURCE_FILE_FOLDER}/Async/CallbackProvider.cpp
  ${SOURCE_FILE_FOLDER}/Async/CallbackProvider.hpp
  ${SOURCE_FILE_FOLDER}/Async/Utils.hpp
  ${SOURCE_FILE_FOLDER}/Async/Utils.cpp
  ${SOURCE_FILE_FOLDER}/IObservable.hpp
  )

target_include_directories(Async PUBLIC ${CMAKE_SOURCE_DIR}/${SOURCE_FILE_FOLDER})

# Test
#add_subdirectory(Source/Tests)

# Example
add_executable(TaskLoopExample ${SOURCE_FILE_FOLDER}/Examples/taskLoopExample.cpp)
target_link_libraries(TaskLoopExample Async)