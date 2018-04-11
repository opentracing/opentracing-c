cmake_minimum_required(VERSION 3.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain.cmake"
    CACHE FILEPATH "Toolchain to use for building this package")

project(opentracing-c VERSION 0.0.1)

include(CheckCCompilerFlag)
include(CheckIncludeFile)
include(CheckTypeSize)
include(CMakeDependentOption)
include(CTest)
include(GenerateExportHeader)

include(CheckAttributes)
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
add_c_flag(flags "-Werror")
add_c_flag(flags "-pedantic")
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

check_attributes()

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/opentracing-c/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/src/opentracing-c/config.h" @ONLY)

foreach(lib opentracingc opentracingc-static)
  target_include_directories(${lib} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>)
  target_compile_options(${lib} PRIVATE ${flags})

  if(OPENTRACINGC_HAVE_WEAK_SYMBOLS)
    target_link_libraries(${lib} PUBLIC dl)
  endif()
endforeach()

option(OPENTRACINGC_LINT "Run clang-tidy on source files" ON)
find_program(CLANG_TIDY_PATH "clang-tidy")
cmake_dependent_option(
    OPENTRACINGC_LINT
    "Enable running clang-tidy on sources during compilation"
    ON
    "CLANG_TIDY_PATH"
    OFF)
if(OPENTRACINGC_LINT)
    set_target_properties(opentracingc PROPERTIES
      C_CLANG_TIDY "${CLANG_TIDY_PATH}")
endif()

if(BUILD_TESTING)
  set(test_src
    "test/dynamic_load_test.c"
    "test/tracer_test.c")

  foreach(test_case_src ${test_src})
    get_filename_component(test_component ${test_case_src} NAME_WE)
    add_executable(${test_component} "${test_case_src}")
    target_link_libraries(${test_component} PUBLIC opentracingc-static)
    add_test(${test_component} ${test_component})
    list(APPEND test_executables ${test_component})
  endforeach()

  if(OPENTRACINGC_HAVE_WEAK_SYMBOLS)
    add_library(mock_tracing_lib SHARED "test/mock_tracing_lib.c")
    target_compile_definitions(mock_tracing_lib PRIVATE DEFINE_HOOK)
    target_link_libraries(mock_tracing_lib PUBLIC opentracingc)

    add_library(mock_tracing_lib_no_hook SHARED "test/mock_tracing_lib.c")
    target_link_libraries(mock_tracing_lib_no_hook PUBLIC opentracingc)

    add_library(mock_tracing_lib_bad_version SHARED "test/mock_tracing_lib.c")
    target_compile_definitions(mock_tracing_lib_bad_version PRIVATE
      BAD_VERSION DEFINE_HOOK)
    target_link_libraries(mock_tracing_lib_bad_version PUBLIC opentracingc)

    add_library(mock_tracing_lib_hook_return_null SHARED
      "test/mock_tracing_lib.c")
    target_compile_definitions(mock_tracing_lib_hook_return_null PRIVATE
      RETURN_NULL_NO_ERROR DEFINE_HOOK)
    target_link_libraries(mock_tracing_lib_hook_return_null PUBLIC opentracingc)

    file(GENERATE
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/test/mock_tracing_lib_names.h"
      INPUT "${CMAKE_CURRENT_SOURCE_DIR}/test/mock_tracing_lib_names.h.in")

    target_include_directories(dynamic_load_test PUBLIC
      "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/test")
  endif()

  if(OPENTRACINGC_COVERAGE)
    target_compile_options(opentracingc-static PRIVATE ${coverage_flags})
    setup_target_for_coverage(NAME unit_test_coverage EXECUTABLE ${test_executables})
  endif()
endif()

option(OPENTRACINGC_BUILD_SNIPPETS "Build opentracing-c snippets" ON)
if(OPENTRACINGC_BUILD_SNIPPETS)
  add_executable(example
    snippets/existing_span.c
    snippets/extract.c
    snippets/inject.c
    snippets/main.c
    snippets/new_span.c
    snippets/text_map.c
    snippets/text_map_iterator.c
    snippets/tracing_impl.c)
  target_include_directories(example PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/snippets>)
  target_link_libraries(example opentracingc)

  set(snippet_files
    snippets/existing_span_snippet.c
    snippets/extract_snippet.c
    snippets/inject_snippet.c
    snippets/main_snippet.c
    snippets/new_span_snippet.c)
  foreach(snippet_file ${snippet_files})
    get_filename_component(snippet_name ${snippet_file} NAME_WE)
    file(READ ${snippet_file} ${snippet_name})
  endforeach()

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/README.adoc.in
    ${CMAKE_CURRENT_SOURCE_DIR}/README.adoc)
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
#   * <prefix>/lib/cmake/opentracing-c/opentracing-cTargets.cmake
install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)
