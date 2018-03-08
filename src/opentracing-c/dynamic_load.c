#include <opentracing-c/dynamic_load.h>

#include <assert.h>
#include <dlfcn.h>
#include <string.h>

void opentracing_library_handle_destroy(opentracing_library_handle* handle)
{
    assert(handle != NULL);
    assert(handle->lib_handle != NULL);
    handle->factory = NULL;
    dlclose(handle->lib_handle);
    handle->lib_handle = NULL;
}

opentracing_dynamic_load_error_code
opentracing_dynamically_load_tracing_library(const char* lib,
                                             opentracing_library_handle* handle,
                                             char* error_buffer,
                                             int error_buffer_length)
{
    opentracing_dynamic_load_error_code (*make_tracer_factory)(
        const char*, opentracing_tracer_factory, char*, int);
    opentracing_dynamic_load_error_code return_code;
    char* error;
    int error_len;
    int error_out_len;

#define COPY_ERROR()                                          \
    do {                                                      \
        if (error != NULL) {                                  \
            error_len = strlen(error);                        \
            error_out_len = (error_len < error_buffer_length) \
                                ? error_len                   \
                                : error_buffer_length;        \
            memcpy(error_buffer, error, error_out_len);       \
        }                                                     \
    } while (0)

    assert(lib != NULL);
    assert(handle != NULL);
    assert(error_buffer != NULL || error_buffer_length == 0);

    make_tracer_factory = NULL;
    return_code = opentracing_dynamic_load_error_code_success;
    error = NULL;
    error_len = 0;
    error_out_len = 0;

    handle->lib_handle = dlopen(lib, RTLD_NOW | RTLD_LOCAL);
    if (handle->lib_handle == NULL) {
        error = dlerror();
        return_code = opentracing_dynamic_load_error_code_failure;
        goto cleanup;
    }

    dlerror(); /* Clear any existing error. */
    *(void**) (&make_tracer_factory) =
        dlsym(handle->lib_handle, "opentracing_make_tracer_factory");
    if (make_tracer_factory == NULL) {
        error = dlerror();
        return_code = opentracing_dynamic_load_error_code_failure;
        goto cleanup;
    }

    return_code = (*make_tracer_factory)(OPENTRACINGC_VERSION_STRING,
                                         handle->factory,
                                         error_buffer,
                                         error_buffer_length);
    if (return_code != 0) {
        goto cleanup;
    }

    if (handle->factory == NULL) {
        error = "Tracer factory is NULL, no error code specified";
        return_code = opentracing_dynamic_load_error_code_failure;
        goto cleanup;
    }

cleanup:
    if (return_code != opentracing_dynamic_load_error_code_success) {
        if (handle->lib_handle != NULL) {
            dlclose(handle->lib_handle);
            handle->lib_handle = NULL;
        }
        COPY_ERROR();
    }
    return return_code;

#undef COPY_ERROR
}
