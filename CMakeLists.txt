cmake_minimum_required(VERSION 3.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain.cmake"
    CACHE FILEPATH "Toolchain to use for building this package")

project(opentracing-c VERSION 0.0.1 LANGUAGES C)

include(CheckCCompilerFlag)
include(CTest)

include(GenerateDocumentation)

function(add_c_flag flags_var new_flag)
  string(MAKE_C_IDENTIFIER "have${new_flag}" new_flag_var)
  check_c_compiler_flag(${new_flag} ${new_flag_var})
  if(${new_flag_var})
    set(${flags_var} ${${flags_var}} ${new_flag} PARENT_SCOPE)
  endif()
endfunction()

add_c_flag(flags "-Wc++-compat")
if("${CMAKE_C_COMPILER}" MATCHES "Clang")
  add_c_flag(flags "-Weverything")
else()
  add_c_flag(flags "-Wall")
  add_c_flag(flags "-Wextra")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(debug_build ON)
endif()
cmake_dependent_option(OPENTRACINGC_COVERAGE "Enable code coverage" OFF
                       "BUILD_TESTING;debug_build" OFF)
if(OPENTRACINGC_COVERAGE)
  include(CodeCoverage)
  append_coverage_compiler_flags(coverage_flags)
  string(REPLACE " " ";" coverage_flags "${coverage_flags}")
  set(COVERAGE_EXCLUDES "test/*")
endif()

set(srcs
  "src/opentracing-c/common.h"
  "src/opentracing-c/destructible.h"
  "src/opentracing-c/propagation.h"
  "src/opentracing-c/span.h"
  "src/opentracing-c/tracer.c"
  "src/opentracing-c/tracer.h"
  "src/opentracing-c/value.h")

add_library(opentracingc-static STATIC ${srcs})
set_target_properties(opentracingc-static PROPERTIES OUTPUT_NAME opentracingc)

add_library(opentracingc SHARED ${srcs})

foreach(lib opentracingc opentracingc-static)
  target_include_directories(${lib} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
    $<INSTALL_INTERFACE:include>)
  target_compile_options(${lib} PUBLIC ${flags})
endforeach()

if(BUILD_TESTING)
  target_compile_options(opentracingc-static PUBLIC ${coverage_flags})
  add_executable(tracer_test "test/tracer_test.c")
  target_link_libraries(tracer_test PUBLIC opentracingc-static)
  add_test(tracer_test tracer_test)
  if(OPENTRACINGC_COVERAGE)
    setup_target_for_coverage(NAME tracer_test_coverage EXECUTABLE tracer_test)
  endif()
endif()

generate_documentation()
