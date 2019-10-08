cmake_minimum_required (VERSION 2.6)
project (AlgoDiff)

option(_DEBUG_ "Enable debugging output" ON)

set (AD_VERSION_MAJOR 1)
set (AD_VERSION_MINOR 0)
 
#add_subdirectory(src)
#add_subdirectory(include)
#add_subdirectory(test)

set(ADIFF_SOURCES src/symbolic.cc src/util.cc src/function.cc )
set(ADIFF_HEADERS include/diff.h include/symbolic.h include/util.h include/variable.h)

configure_file (
  "${PROJECT_SOURCE_DIR}/conf/algodiff-config.h.in"
  "${PROJECT_BINARY_DIR}/algodiff-config.h"
  )

include_directories("./include/")
include_directories("${PROJECT_BINARY_DIR}")

add_library(algodiff ${ADIFF_SOURCES} ${ADIFF_HEADERS})

# test executables
add_executable(test.variable.nn test/test-variable-nn.cc)
target_link_libraries(test.variable.nn algodiff)

add_executable(test.variable.compose test/test-variable-compose.cc)
target_link_libraries(test.variable.compose algodiff)

add_executable(test.sym.assign test/test-sym-assign.cc)
target_link_libraries(test.sym.assign algodiff)

add_executable(test.sym.multi test/test-sym-multinom.cc)
target_link_libraries(test.sym.multi algodiff)

add_executable(test.sym.nn test/test-sym-nn.cc)
target_link_libraries(test.sym.nn algodiff)
