if(__CHECK_ATTRIBUTES)
  return()
endif()
set(__CHECK_ATTRIBUTES 1)

function(__check_specific_attribute specific_attribute var)
  try_compile(have_${specific_attribute}_attribute
    "${tmp_dir}/${specific_attribute}_attribute_test"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/specific_attribute_test.c"
    COMPILE_DEFINITIONS "-DATTRIBUTE=${OPENTRACINGC_ATTRIBUTE}"
                        "-DX=${specific_attribute}")

  if(have_${specific_attribute}_attribute)
    set(${var} ON PARENT_SCOPE)
    message(STATUS "Checking for ${OPENTRACINGC_ATTRIBUTE}((${specific_attribute})) - Success")
  else()
    message(STATUS "Checking for ${OPENTRACINGC_ATTRIBUTE}((${specific_attribute})) - Failed")
  endif()

endfunction()

function(__check_have_weak_symbols var)
  __check_specific_attribute("weak" have_weak_symbols)

  if(have_weak_symbols)
    set(${var} ON PARENT_SCOPE)
    message(STATUS "Checking for weak symbol support - Success")
  else()
    message(STATUS "Checking for weak symbol support - Failed")
  endif()
endfunction()

function(__check_have_nonnull_attribute var)
  __check_specific_attribute("nonnull" have_nonnull_attribute)
  if(have_nonnull_attribute)
    set(${var} ON PARENT_SCOPE)
  endif()
endfunction()

function(check_attributes)
  if(NOT DEFINED OPENTRACINGC_ATTRIBUTE)
    set(tmp_dir "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp")
    foreach(keyword "__attribute__" "__attribute")
      try_compile(have_${keyword}
        "${tmp_dir}/attribute_keyword_test"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/attribute_keyword_test.c"
        COMPILE_DEFINITIONS "-DATTRIBUTE=${keyword}")
      if(have_${keyword})
        set(OPENTRACINGC_ATTRIBUTE "${keyword}" CACHE INTERNAL
            "Attribute keyword")
        break()
      endif()
    endforeach()
  endif()

  if(NOT OPENTRACINGC_ATTRIBUTE)
    return()
  endif()

  __check_have_weak_symbols(have_weak_symbols)
  if(have_weak_symbols)
    set(OPENTRACINGC_HAVE_WEAK_SYMBOLS ON PARENT_SCOPE)
  endif()

  __check_have_nonnull_attribute(have_nonnull_attribute)
  if(have_nonnull_attribute)
    set(OPENTRACINGC_HAVE_NONNULL_ATTRIBUTE ON PARENT_SCOPE)
  endif()
endfunction()
