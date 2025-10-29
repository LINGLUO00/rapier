cmake_minimum_required(VERSION 4.1)
project(rapier)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGES} -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

include_directories(.)
include_directories(/usr/include)
link_directories(/usr/lib/)

set(LIB_SRC
  src/log.cc
  src/util.cc
  src/config.cc
)

add_library(rapier SHARED ${LIB_SRC})
#add_library(rapier_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES(rapier_static PROPERTIES OUTPUT_NAME "rapier")

add_executable(test tests/test.cc)
add_dependencies(test rapier)
target_link_libraries(test rapier yaml-cpp)

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config rapier)
target_link_libraries(test_config rapier yaml-cpp )


SET(EXECTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
