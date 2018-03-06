if(__GENERATE_DOCUMENTATION)
  return()
endif()
set(__GENERATE_DOCUMENTATION ON)

include(CMakeDependentOption)

function(generate_documentation)
  find_package(Doxygen)
  cmake_dependent_option(OPENTRACINGC_BUILD_DOC "Build documentation" ON
                         "DOXYGEN_EXECUTABLE" OFF)
  if(NOT OPENTRACINGC_BUILD_DOC)
    return()
  endif()

  get_target_property(defs opentracingc COMPILE_DEFINITIONS)
  get_target_property(inc opentracingc INCLUDE_DIRECTORIES)

  set(doxygen_out_path "${CMAKE_CURRENT_BINARY_DIR}/doc/$<CONFIG>")

  file(READ "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in" doxyfile_content)
  string(CONFIGURE "${doxyfile_content}" doxyfile_content @ONLY)
  file(GENERATE OUTPUT "${doxygen_out_path}/Doxyfile"
       CONTENT "${doxyfile_content}")

  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} "${doxygen_out_path}/Doxyfile"
    DEPENDS "${doxygen_out_path}/Doxyfile"
            "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    USES_TERMINAL)
endfunction()
