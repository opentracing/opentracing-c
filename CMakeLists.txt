cmake_minimum_required(VERSION 3.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain.cmake"
    CACHE FILEPATH "Toolchain to use for building this package")

project(opentracing-c VERSION 0.0.1)

include(CheckCCompilerFlag)
include(CheckIncludeFile)
include(CheckTypeSize)
include(CTest)
include(GenerateExportHeader)

include(CheckHaveWeakSymbols)
include(GenerateDocumentation)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
  "src/opentracing-c/dynamic_load.c"
  "src/opentracing-c/dynamic_load.h"
  "src/opentracing-c/propagation.h"
  "src/opentracing-c/span.h"
  "src/opentracing-c/tracer.c"
  "src/opentracing-c/tracer.h"
  "src/opentracing-c/value.h")

add_library(opentracingc-static STATIC ${srcs})
add_library(opentracingc SHARED ${srcs})
generate_export_header(opentracingc
  EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/src/opentracing-c/visibility.h")
set_target_properties(opentracingc-static PROPERTIES
  OUTPUT_NAME opentracingc
  COMPILE_DEFINITIONS OPENTRACINGC_STATIC_DEFINE)
set_target_properties(opentracingc PROPERTIES
  C_VISIBILITY_INLINES hidden
  C_VISIBILITY_PRESET hidden)

check_include_file("sys/time.h" HAVE_SYS_TIME_H)
check_type_size("struct timespec" OPENTRACINGC_USE_TIMESPEC)
check_have_weak_symbols()

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/opentracing-c/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/src/opentracing-c/config.h" @ONLY)

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

# Installation (https://github.com/forexample/package-example)

# Layout. This works for all platforms:
#   * <prefix>/lib/cmake/<PROJECT-NAME>
#   * <prefix>/lib/
#   * <prefix>/include/
set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Use:
#   * PROJECT_VERSION
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * TARGETS_EXPORT_NAME
#   * PROJECT_NAME
configure_package_config_file(
    "cmake/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)

# Targets:
#   * <prefix>/lib/libopentracingc.so
#   * <prefix>/lib/libopentracingc.a
#   * header location after install: <prefix>/include/opentracing-c/tracer.h
#   * headers can be included by C++ code `#include <opentracing-c/tracer.h>`
install(
    TARGETS opentracingc opentracingc-static
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "${include_install_dir}"
)

# Headers:
#   * src/opentracing-c/tracer.h -> <prefix>/include/src/tracer.h
install(
    DIRECTORY "src/opentracing-c"
    DESTINATION "${include_install_dir}"
    FILES_MATCHING PATTERN "*.h"
)

# Generated headers:
#   * ${CMAKE_CURRENT_BINARY_DIR}/.../opentracing-c/config.h ->
#     <prefix>/include/opentracing-c/config.h
install(
    DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src/opentracing-c"
    DESTINATION "${include_install_dir}"
    FILES_MATCHING PATTERN "*.h"
)

# Config
#   * <prefix>/lib/cmake/opentracing-c/opentracing-cConfig.cmake
#   * <prefix>/lib/cmake/opentracing-c/opentracing-cConfigVersion.cmake
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

# Config
#   * <prefix>/lib/cmake/Foo/FooTargets.cmake
install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)
