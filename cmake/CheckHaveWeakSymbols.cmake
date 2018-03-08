if(__CHECK_HAVE_WEAK_SYMBOLS)
  return()
endif()
set(__CHECK_HAVE_WEAK_SYMBOLS 1)

function(check_have_weak_symbols)
  if(DEFINED OPENTRACINGC_HAVE_WEAK_SYMBOLS)
    return()
  endif()

  try_compile(OPENTRACINGC_HAVE_WEAK_SYMBOLS
    "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/weak_symbols_test"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/weak_symbols_test.c")

  if(OPENTRACINGC_HAVE_WEAK_SYMBOLS)
    message(STATUS "Checking for weak symbol support - Success")
  else()
    message(STATUS "Checking for weak symbol support - Failed")
  endif()
endfunction()
