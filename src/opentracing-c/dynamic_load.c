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
#define COPY_ERROR()                                                  \
    do {                                                              \
        if (error != NULL && error_buffer != NULL) {                  \
            error_len = strlen(error) + 1;                            \
            if (error_len > error_buffer_length) {                    \
                memcpy(error_buffer, error, error_buffer_length - 1); \
                error_buffer[error_buffer_length - 1] = '\0';         \
            }                                                         \
            else {                                                    \
                memcpy(error_buffer, error, error_len);               \
            }                                                         \
        }                                                             \
    } while (0)

    const char* error;
    int error_len;

#ifdef OPENTRACINGC_HAVE_WEAK_SYMBOLS

    opentracing_tracer_factory (*make_tracer_factory)(
        const char*, opentracing_dynamic_load_error_code*, char*, int);
    opentracing_dynamic_load_error_code return_code;

    assert(lib != NULL);
    assert(handle != NULL);
    assert(error_buffer != NULL || error_buffer_length == 0);

    make_tracer_factory = NULL;
    return_code = opentracing_dynamic_load_error_code_success;
    error = NULL;

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

    handle->factory = (*make_tracer_factory)(OPENTRACINGC_VERSION_STRING,
                                             &return_code,
                                             error_buffer,
                                             error_buffer_length);
    if (return_code != 0) {
        assert(error == NULL);
        goto cleanup;
    }

    if (handle->factory == NULL) {
        error = "Tracer factory is NULL, no error code specified";
        return_code = opentracing_dynamic_load_error_code_failure;
        goto cleanup;
    }

cleanup:
    if (return_code != opentracing_dynamic_load_error_code_success) {
        COPY_ERROR();
        if (handle->lib_handle != NULL) {
            dlclose(handle->lib_handle);
            handle->lib_handle = NULL;
        }
    }
    return return_code;

#else

    error = "Platform has no weak symbol support";
    COPY_ERROR();
    return opentracing_dynamic_load_error_code_not_supported;

#endif /* OPENTRACINGC_HAVE_WEAK_SYMBOLS */

#undef COPY_ERROR
}
