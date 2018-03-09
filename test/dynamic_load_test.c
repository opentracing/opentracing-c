#include <assert.h>
#include <string.h>

#include <opentracing-c/dynamic_load.h>

#ifdef OPENTRACINGC_HAVE_WEAK_SYMBOLS
#include "mock_tracing_lib_names.h"
#endif /* OPENTRACINGC_HAVE_WEAK_SYMBOLS */

int main(void)
{
    opentracing_library_handle handle;
    char error[256];
    opentracing_dynamic_load_error_code error_code;

#ifdef OPENTRACINGC_HAVE_WEAK_SYMBOLS
    opentracing_tracer* tracer;
#endif /* OPENTRACINGC_HAVE_WEAK_SYMBOLS */

    memset(&handle, 0, sizeof(handle));
    memset(&error, 0, sizeof(error));

    error_code = opentracing_dynamically_load_tracing_library(
        "libdoesnotexist.so", &handle, error, sizeof(error));

#ifdef OPENTRACINGC_HAVE_WEAK_SYMBOLS

    assert(error_code == opentracing_dynamic_load_error_code_failure);
    assert(handle.factory == NULL);
    assert(handle.lib_handle == NULL);

    error_code = opentracing_dynamically_load_tracing_library(
        MOCK_TRACING_LIB_NO_HOOK_NAME, &handle, error, sizeof(error));
    assert(error_code == opentracing_dynamic_load_error_code_failure);
    assert(handle.factory == NULL);
    assert(handle.lib_handle == NULL);

    error_code = opentracing_dynamically_load_tracing_library(
        MOCK_TRACING_LIB_BAD_VERSION_NAME, &handle, error, sizeof(error));
    assert(error_code ==
           opentracing_dynamic_load_error_code_incompatible_library_versions);
    assert(handle.factory == NULL);
    assert(handle.lib_handle == NULL);

    error_code = opentracing_dynamically_load_tracing_library(
        MOCK_TRACING_LIB_NAME, &handle, error, sizeof(error));
    assert(error_code == opentracing_dynamic_load_error_code_success);
    assert(handle.factory != NULL);
    assert(handle.lib_handle != NULL);
    assert((*handle.factory)("", &tracer, error, sizeof(error)) ==
           opentracing_true);
    ((opentracing_destructible*) tracer)
        ->destroy((opentracing_destructible*) tracer);
    opentracing_library_handle_destroy(&handle);

#else

    assert(error_code == opentracing_dynamic_load_error_code_not_supported);
    assert(handle.factory == NULL);
    assert(handle.lib_handle == NULL);

#endif /* OPENTRACINGC_HAVE_WEAK_SYMBOLS */
    return 0;
}
